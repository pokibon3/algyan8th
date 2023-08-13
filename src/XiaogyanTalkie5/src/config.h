#include <freertos/FreeRTOS.h>
#include <driver/i2s.h>
#include <driver/gpio.h>

// WiFi credentials
//#define WIFI_SSID 
//#define WIFI_PSWD 
#define USE_ESP_NOW
// sample rate for the system
#define SAMPLE_RATE 16000
//ESPNow Long Range Mode
#define ESPNOW_LONG_RANGE

// Which channel is the I2S microphone on? I2S_CHANNEL_FMT_ONLY_LEFT or I2S_CHANNEL_FMT_ONLY_RIGHT
// Generally they will default to LEFT - but you may need to attach the L/R pin to GND
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ALL_RIGHT
#define I2S_MIC_SERIAL_CLOCK    6
#define I2S_MIC_SERIAL_DATA     5

// speaker settings
#define PWM_SPEAKER_PIN         D0
#define PWM_SPEAKER_ENABLE_PIN  -1
#define PWM_SPEAKER_LEDC_CHANNEL 0

// transmit button
#define GPIO_TRANSMIT_BUTTON    D9         

// On which wifi channel (1-11) should ESP-Now transmit? The default ESP-Now channel on ESP32 is channel 1
#define ESP_NOW_WIFI_CHANNEL    1

// i2s config for reading from of I2S
extern i2s_config_t i2s_mic_Config;
// i2s microphone pins
extern i2s_pin_config_t i2s_mic_pins;
// i2s speaker pins
extern i2s_pin_config_t i2s_speaker_pins;
