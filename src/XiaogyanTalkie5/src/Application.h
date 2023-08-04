#pragma once

//class Output;
class Speaker;
//class I2SSampler;
class PDMMic;
class Transport;
class OutputBuffer;

class Application
{
private:
    Speaker         *m_speaker;
    PDMMic          *m_input;
    Transport       *m_transport;
    OutputBuffer    *m_output_buffer;

public:
    Application();
    void dispRSSI();
    void begin();
    void loop();
};
