#pragma once

//class Output;
class Speaker;
class I2SSampler;
class Transport;
class OutputBuffer;
class IndicatorLed;

class Application
{
private:
//  Output *m_output;
  Speaker     *m_speaker;
  I2SSampler  *m_input;
  Transport   *m_transport;
//  IndicatorLed *m_indicator_led;
  OutputBuffer *m_output_buffer;

public:
  Application();
  void begin();
  void loop();
};
