#include <Arduino.h>
#include <driver/i2s.h>

#define PIN_CLK     6
#define PIN_DATA    5
#define READ_LEN    (2 * 256)
#define GAIN_FACTOR 3
uint8_t BUFFER[READ_LEN] = {0};

int16_t *adcBuffer = NULL;

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
        Serial.println(adcBuffer[i]);
    }
}

void mic_record_task(void *arg) {
    size_t bytesread;
    while (1) {
        i2s_read(I2S_NUM_0, (char *)BUFFER, READ_LEN, &bytesread,
                 (100 / portTICK_RATE_MS));
        adcBuffer = (int16_t *)BUFFER;
        showSignal();
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}
// After M5StickC is started or reset
void setup() {
    Serial.begin();
    i2sInit();
    xTaskCreate(mic_record_task, "mic_record_task", 2048, NULL, 1, NULL);
}
// After the program in setup() runs, it runs the program in loop()

void loop() {
    //printf("loop cycling\n");
    vTaskDelay(1000 / portTICK_RATE_MS);  // otherwise the main task wastes half
                                          // of the cpu cycles
}