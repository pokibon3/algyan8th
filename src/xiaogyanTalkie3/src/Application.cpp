#include <Arduino.h>
#include <driver/i2s.h>
#include <WiFi.h>

#include "Application.h"
#include "I2SMEMSSampler.h"
//#include "ADCSampler.h"
//#include "I2SOutput.h"
//#include "DACOutput.h"
#include "UdpTransport.h"
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
  m_input = new I2SMEMSSampler(I2S_NUM_0, i2s_mic_pins, i2s_mic_Config,128);

  m_speaker = new Speaker(PWM_SPEAKER_PIN, PWM_SPEAKER_ENABLE_PIN, PWM_SPEAKER_LEDC_CHANNEL);
  m_transport = new EspNowTransport(m_output_buffer,ESP_NOW_WIFI_CHANNEL);
  m_transport->set_header(TRANSPORT_HEADER_SIZE,transport_header);
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
//  xTaskCreateUniversal(application_task, "application_task", 8192, this, 1, &task_handle, CONFIG_ARDUINO_RUNNING_CORE);
}

// application task - coordinates everything
uint8_t BUFFER[256] = {0};
uint16_t *micBuffer = NULL;

void Application::loop()
{
//  int16_t *samples = reinterpret_cast<int16_t *>(malloc(sizeof(int16_t) * 128));
  micBuffer = (uint16_t *)BUFFER;

  // continue forever
  while (true) {
    // do we need to start transmitting?
    int16_t ptt = digitalRead(GPIO_TRANSMIT_BUTTON);
    if (!ptt) {
      Serial.println("Started transmitting");
      // stop the output as we're switching into transmit mode
      m_speaker->stop();
      // start the input to get samples from the microphone
      m_input->start();
      // transmit for at least 1 second or while the button is pushed
      unsigned long start_time = millis();
      while (millis() - start_time < 1000 || !digitalRead(GPIO_TRANSMIT_BUTTON)) {
        // read samples from the microphone
        size_t samples_read = 0;
        i2s_read(I2S_NUM_0, (char *)BUFFER, 256, &samples_read, portMAX_DELAY);
//debug        m_speaker->play(micBuffer, 128, SAMPLE_RATE);

        // and send them over the transport
        for (int i = 0; i < samples_read / 2; i++) {
          uint16_t sample = micBuffer[i] >> 2;
          m_transport->add_sample(sample & 0x00ff);
          m_transport->add_sample((sample & 0xff00) >> 8);
//          m_transport->add_sample(BUFFER[i]);
        }
//        break;      // debug
      }
      m_transport->flush();
      // finished transmitting stop the input and start the output
      Serial.println("Finished transmitting");
      m_input->stop();
    }
    // while the transmit button is not pushed and 1 second has not elapsed
    Serial.println("Started Receiving");
    unsigned long start_time = millis();
    while (millis() - start_time < 1000 || digitalRead(GPIO_TRANSMIT_BUTTON))
    {
      // read from the output buffer (which should be getting filled by the transport)
      m_output_buffer->remove_samples(BUFFER, 256);
      m_speaker->play(micBuffer, 128, SAMPLE_RATE);
      for (int i = 0; i < 128; i++) {
        if (micBuffer[i] != 0 && micBuffer[i] < 10000) {
          Serial.print(micBuffer[i]);
          Serial.println();
//          delay(1);
        }
      }
//      delay(1);
    }
    Serial.println("Finished Receiving");
  }
}
