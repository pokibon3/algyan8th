#include <esp_now.h>
#include <WiFi.h>

// ESPNOW
#define WIFI_CHANNEL 1
#define ESPNOW_CHANNEL WIFI_CHANNEL


// Function for handling received data
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  char macStr[18];
  uint16_t *bufp;
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(macStr);
  Serial.printf("Data (%d bytes): ", len);
  
  // Uncomment below lines if incoming data is known to be printable
  // String dataStr = String((char*)incomingData);
  // Serial.println(dataStr);

  // Or print raw bytes of data
  bufp = (uint16_t *)incomingData;
  for (int i = 0; i < len / 2; i++) {
    Serial.printf("%d ", bufp[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(5000);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("Start Receiving");
  Serial.println(esp_now_register_recv_cb(OnDataRecv));
}

void loop() {
  // Nothing to do here, as we are only receiving ESP-NOW packets
  Serial.println("Receiving");
  delay(1000);
}
