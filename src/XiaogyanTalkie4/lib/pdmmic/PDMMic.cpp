#include <Arduino.h>
#include "driver/i2s.h" //https://github.com/espressif/arduino-esp32/blob/mN_LEN_READ_BYTEaster/tools/sdk/include/driver/driver/i2s.h
#include "PDMmic.hpp"

#define I2S_MIC_SERIAL_CLOCK    6
#define I2S_MIC_SERIAL_DATA     5

#define HZ_SAMPLE_RATE 16000

// i2s config for reading from I2S
i2s_config_t i2s_mic_Config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
    .sample_rate = HZ_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
};

// i2s microphone pins
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_PIN_NO_CHANGE,
    .ws_io_num = I2S_MIC_SERIAL_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA
};


PDMMic::PDMMic(void) 
{
    offset = 0;
}

void PDMMic::start(void) 
{
    i2s_driver_install(I2S_NUM_0, &i2s_mic_Config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);
    i2s_set_clk(I2S_NUM_0, HZ_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}

void PDMMic::read(uint8_t *buf, size_t bufSize, size_t *readSize)
{
    int16_t *sample = (int16_t *)buf;
    int32_t newOffset = 0;

    i2s_read(I2S_NUM_0, buf, bufSize, readSize, portMAX_DELAY);
    int16_t count = *readSize /2;
    for (int i = 0; i < count; i++) {
        newOffset += sample[i];
        sample[i] -= offset;
//        Serial.println(sample[i]);
    }
    newOffset /= count;
//    Serial.printf("offset : %d, %d\n", offset, newOffset);
    offset = newOffset;
}

void PDMMic::stop(void) 
{
    // stop the i2S driver
    i2s_driver_uninstall(I2S_NUM_0);
}