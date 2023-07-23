#include <Arduino.h>
#include <esp_now.h>
#include <WIFI.h>
#include <driver/i2s.h>

// Define ESPNOW parameters
#define ESPNOW_CHANNEL 1
#define WIFI_CHANNEL ESPNOW_CHANNEL

// Define I2S parameters
#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 16000
#define BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_16BIT
#define I2S_CHANNEL_NUM I2S_CHANNEL_MONO
#define PDM_MIC_CLK 6
#define PDM_MIC_DATA 5

// ESPNOW
esp_now_peer_info_t peerInfo;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//  Serial.print("\r\nLast Packet Send Status:\t");
//  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Initialize ESPNOW
void ESPNOW_Init() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = ESPNOW_CHANNEL;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != 0) {
    Serial.println("Failed to add peer");
    return;
  }
}

i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = BITS_PER_SAMPLE,
    .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 2,
//    .dma_buf_len = 1024,
    .dma_buf_len = 120,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
};

// Initialize I2S (PDM microphone)
void I2S_Init() {

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_PIN_NO_CHANGE,
    .ws_io_num = PDM_MIC_CLK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = PDM_MIC_DATA
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_set_clk(I2S_PORT, SAMPLE_RATE, BITS_PER_SAMPLE, I2S_CHANNEL_NUM);
}

void setup() {
  Serial.begin(115200);
  delay(5000);
  ESPNOW_Init();
  I2S_Init();
}

void loop() {
  uint8_t i2s_read_buff[240];
  uint16_t *bufp;
  size_t bytes_read;
  esp_now_peer_info_t esp_now_temp;

  bufp = (uint16_t *)i2s_read_buff;
  // Read samples from I2S
  i2s_read(I2S_PORT, i2s_read_buff, sizeof(i2s_read_buff), &bytes_read, portMAX_DELAY);
//  for (int i = 0; i < bytes_read /2 ; i++) {
//    Serial.println(bufp[i]);
//  }
  // Send samples via ESP-NOW
  //Serial.println(bytes_read);

  // peerリストに無い場合は追加する
  for (int i = 0; i < 6; i++ )   esp_now_temp.peer_addr[i] = (uint8_t) broadcastAddress[i];
  esp_now_temp.channel = ESPNOW_CHANNEL; // pick a channel
  esp_now_temp.encrypt = 0;       // no encryption  
  if( !esp_now_is_peer_exist(esp_now_temp.peer_addr) ){
    esp_err_t result = esp_now_add_peer(&esp_now_temp);    // Master not paired, attempt pair
    if(result!=ESP_OK ){
      Serial.println("Error add peer address");
      return;
    }
  }
  
  esp_err_t result = esp_now_send(broadcastAddress, i2s_read_buff, bytes_read);

  if (result == ESP_OK) {
//    Serial.println("Sent data successfully");
  } else {
    Serial.println("Error sending the data");
  }
}
