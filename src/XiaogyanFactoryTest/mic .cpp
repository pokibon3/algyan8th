#include <Arduino.h>
#include "driver/i2s.h" //https://github.com/espressif/arduino-esp32/blob/mN_LEN_READ_BYTEaster/tools/sdk/include/driver/driver/i2s.h

#define PIN_I2S_WS  6
#define PIN_I2S_DIN 5
//#define PIN_I2S_WS GPIO_NUM_42
//#define PIN_I2S_DIN GPIO_NUM_41

#define HZ_SAMPLE_RATE 16000

#define N_LEN_READ_2BYTE 1000 
#define N_BYTES_STRACT_PAR_SAMPLE 2
#define N_LEN_READ_BYTES  (N_LEN_READ_2BYTE*N_BYTES_STRACT_PAR_SAMPLE)

void setup(void) {
  esp_err_t erResult = ESP_OK;
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX| I2S_MODE_PDM),
    .sample_rate = HZ_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
#if ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 1, 0)
    .communication_format =
          I2S_COMM_FORMAT_STAND_I2S,  // Set the format of the communication.
#else                                   // 设置通讯格式
        .communication_format = I2S_COMM_FORMAT_I2S,
#endif
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 2,
    .dma_buf_len = 128,
    .use_apll = false,
    .tx_desc_auto_clear   = false,
    .fixed_mclk           = 0
  };
  erResult = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);

  i2s_pin_config_t pin_config;
  pin_config.bck_io_num = I2S_PIN_NO_CHANGE;
  pin_config.ws_io_num = PIN_I2S_WS;
  pin_config.data_out_num = I2S_PIN_NO_CHANGE;
  pin_config.data_in_num = PIN_I2S_DIN;
  erResult = i2s_set_pin(I2S_NUM_0, &pin_config);
  erResult = i2s_set_clk(I2S_NUM_0, HZ_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}

void loop() {
  //I2S_Init();
  size_t uiGotLen=0;
  uint8_t u8buf[N_LEN_READ_BYTES];
  esp_err_t erReturns = i2s_read(I2S_NUM_0, (char *)u8buf, N_LEN_READ_BYTES, &uiGotLen, portMAX_DELAY);
  uint16_t * u16buf = (uint16_t *)&u8buf[0];

  Serial.begin(115200);
  for (int i=0;i<N_LEN_READ_2BYTE;i++)
    Serial.println(u16buf[i]);

}