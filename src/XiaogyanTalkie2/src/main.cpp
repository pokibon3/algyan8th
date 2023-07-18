#include <Arduino.h>
#include <driver/i2s.h>
#include "Speaker.hpp"

#define PIN_CLK     6
#define PIN_DATA    5
#define READ_LEN    (2 * 256)
#define GAIN_FACTOR 3
#define SPEAKER_PIN D0
#define SPEAKER_ENABLE_PIN -1
#define SPEAKER_LEDC_CHANNEL 0

uint8_t BUFFER[READ_LEN] = {0};
uint8_t speakerBuffer[256];


uint16_t *micBuffer = NULL;

Speaker speaker(SPEAKER_PIN, SPEAKER_ENABLE_PIN, SPEAKER_LEDC_CHANNEL);
extern const unsigned char shupo_16k_u8_raw[];
extern const unsigned int shupo_16k_u8_raw_len;
extern const uint16_t sin_16k_u8_raw[];
extern const unsigned int sin_16k_u8_raw_len;

void i2sInit()  // Init I2S.  初始化I2S
{
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX |
                             I2S_MODE_PDM),  // Set the I2S operating mode.
        .sample_rate = 16000,  // Set the I2S sampling rate.
        .bits_per_sample =
            I2S_BITS_PER_SAMPLE_16BIT,  // Fixed 12-bit stereo MSB.
        .channel_format =
            I2S_CHANNEL_FMT_ALL_RIGHT,  // Set the channel format.  
        .communication_format =
            I2S_COMM_FORMAT_STAND_I2S,  // Set the format of the communication.
        .intr_alloc_flags =
            ESP_INTR_FLAG_LEVEL1,  // Set the interrupt flag.
        .dma_buf_count = 2,        // DMA buffer count.
        .dma_buf_len   = 128,      // DMA buffer length.
    };

    i2s_pin_config_t pin_config;

    pin_config.mck_io_num = I2S_PIN_NO_CHANGE;

    pin_config.bck_io_num   = I2S_PIN_NO_CHANGE;
    pin_config.ws_io_num    = PIN_CLK;
    pin_config.data_out_num = I2S_PIN_NO_CHANGE;
    pin_config.data_in_num  = PIN_DATA;

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}

void showSignal() {
    for (int i = 0; i < 256; i++){
//        Serial.printf("%u,\n", micBuffer[i]);
        speakerBuffer[i] = (uint8_t)((micBuffer[i] >> 2) & 0x00ff);
//        speakerBuffer[i] = (uint8_t)micBuffer[i];
    }
    speaker.play(speakerBuffer, 256, 32000);
    //delay(4);
}

void mic_record_task(void *arg) {
    size_t bytesread;
    while (1) {
        i2s_read(I2S_NUM_0, (char *)BUFFER, READ_LEN, &bytesread,
                 (100 / portTICK_RATE_MS));
        micBuffer = (uint16_t *)BUFFER;
        showSignal();
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}
// After M5StickC is started or reset
uint8_t buf[16530], buf8;
void setup() {
    Serial.begin();
    i2sInit();
    speaker.begin();
#if 0
    for(int i = 0; i < sin_16k_u8_raw_len; i++) {
        buf8 = (sin_16k_u8_raw[i] >> 2) & 0x00ff;
        buf[i] = buf8;
        Serial.printf("%d : %d\n", sin_16k_u8_raw[i], buf8);
    }
    for (int i = 0; i < 10; i++) {
        speaker.play(buf, sin_16k_u8_raw_len, 4000);
        delay(1000);
    }
    speaker.play(shupo_16k_u8_raw, shupo_16k_u8_raw_len, 16000);
#endif
    xTaskCreate(mic_record_task, "mic_record_task", 2048, NULL, 1, NULL);
}
// After the program in setup() runs, it runs the program in loop()

void loop() {
    //printf("loop cycling\n");
    vTaskDelay(1000 / portTICK_RATE_MS);  // otherwise the main task wastes half
                                          // of the cpu cycles
}