#include <Arduino.h>
#include <driver/i2s.h>
#include <WiFi.h>

#include "Application.h"
#include "PDMMic.hpp"
#include "EspNowTransport.h"
#include "OutputBuffer.h"
#include "config.h"
#include "Speaker.hpp"
#include "Xiaogyan.hpp"

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
//    m_speaker       = new Speaker(PWM_SPEAKER_PIN, PWM_SPEAKER_ENABLE_PIN, PWM_SPEAKER_LEDC_CHANNEL);
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
//    Xiaogyan.speaker.begin();
    // start the main task for the application
    TaskHandle_t task_handle;
    xTaskCreate(application_task, "application_task", 8192, this, 1, &task_handle);
}

void Application::setChannel(uint16_t ch) 
{
    m_transport->setWifiChannel(ch);
}

int16_t rssi_level[] = {
//    -50, -44, -38, -32, -26, -20, -10, 0
//    -56, -50, -44, -38, -32, -26, -20, -10
//    -62, -56, -50, -44, -38, -32, -26, -20
//    -80, -70, -60, -50, -40, -30, -20, -10
    -90, -80, -70, -60, -50, -40, -30, -20
};

void Application::dispRSSI(int16_t rssi)
{
//    Serial.println(rssi);
    Xiaogyan.ledMatrix.setBrightness(5);
    for (int i = 0; i < 8; i++) {
        if (rssi >= rssi_level[i]) {
            Xiaogyan.ledMatrix.drawPixel(i, 7, (i > 4)? 1 : 2);
        } else {
            Xiaogyan.ledMatrix.drawPixel(i, 7, 0);
        }
    }
}

int16_t Application::getRSSI()
{
    return m_transport->getRSSI();
}


// application task - coordinates everything
void Application::loop()
{
    int16_t *samples = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
    uint32_t dispCount = 0;
    while (true) {
        //int16_t ptt = digitalRead(GPIO_TRANSMIT_BUTTON);
        int16_t ptt = Xiaogyan.buttonA.read();
        if (!ptt) {
            Serial.println("Started transmitting");
            dispRSSI(0);                    // meter full scale
            Xiaogyan.speaker.stop();
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
        //while (millis() - start_time < 1000 || digitalRead(GPIO_TRANSMIT_BUTTON)) {
        while (millis() - start_time < 1000 || Xiaogyan.buttonA.read()) {
            if ((dispCount++ % 10) == 0) dispRSSI(getRSSI());
            m_output_buffer->remove_samples((uint8_t *)samples, 128);
            //while(m_speaker->busy());               // until speaker is free
            while(Xiaogyan.speaker.busy()) {               // until speaker is free
                vTaskDelay(pdMS_TO_TICKS(1));
            }
            //m_speaker->play((uint8_t *)samples, 128, SAMPLE_RATE);
            Xiaogyan.speaker.play((uint8_t *)samples, 128, SAMPLE_RATE);
        }
        Serial.println("Finished Receiving");
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
