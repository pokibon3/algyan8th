#include <Arduino.h>
#include <driver/i2s.h>
#include <WiFi.h>

#include "Application.h"
//#include "I2SMEMSSampler.h"
#include "PDMMic.hpp"
#include "EspNowTransport.h"
#include "OutputBuffer.h"
#include "config.h"
#include "Speaker.hpp"

static void application_task(void *param)
{
    // delegate onto the application
    Application *application = reinterpret_cast<Application *>(param);
    application->loop();
}

Application::Application()
{
    m_output_buffer = new OutputBuffer(300 * 16);
    m_input         = new PDMMic();
    m_speaker       = new Speaker(PWM_SPEAKER_PIN, PWM_SPEAKER_ENABLE_PIN, PWM_SPEAKER_LEDC_CHANNEL);
    m_transport     = new EspNowTransport(m_output_buffer,ESP_NOW_WIFI_CHANNEL);
}

void Application::begin()
{
    Serial.print("My IDF Version is: ");
    Serial.println(esp_get_idf_version());

    // bring up WiFi
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    Serial.print("My MAC Address is: ");
    Serial.println(WiFi.macAddress());
    // do any setup of the transport
    m_transport->begin();
    pinMode(GPIO_TRANSMIT_BUTTON, INPUT_PULLUP);    

    // start off with i2S output running
    m_speaker -> begin();
    // start the main task for the application
    TaskHandle_t task_handle;
    xTaskCreate(application_task, "application_task", 8192, this, 1, &task_handle);
}

// application task - coordinates everything
void Application::loop()
{
    int16_t *samples = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));

    while (true) {
        int16_t ptt = digitalRead(GPIO_TRANSMIT_BUTTON);
        if (!ptt) {
            Serial.println("Started transmitting");
            m_speaker->stop();
            m_input->start();
            unsigned long start_time = millis();
            while (millis() - start_time < 1000 || !digitalRead(GPIO_TRANSMIT_BUTTON)) {
                size_t samples_read = 0;
                m_input->read((uint8_t *)samples, 256, &samples_read);
                for (int i = 0; i < samples_read / 2; i++) {
                    m_transport->add_sample(samples[i]);
                }
            }
            m_transport->flush();
            // finished transmitting stop the input and start the output
            Serial.println("Finished transmitting");
            m_input->stop();
        }
        // while the transmit button is not pushed and 1 second has not elapsed
        Serial.println("Started Receiving");
        unsigned long start_time = millis();
        while (millis() - start_time < 1000 || digitalRead(GPIO_TRANSMIT_BUTTON)) {
            m_output_buffer->remove_samples((uint8_t *)samples, 128);
            while(m_speaker->busy());               // until speaker is free
            m_speaker->play((uint8_t *)samples, 128, SAMPLE_RATE);
        }
        Serial.println("Finished Receiving");
    }
}
