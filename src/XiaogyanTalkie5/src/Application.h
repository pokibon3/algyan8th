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
    Speaker     *m_speaker;
//    I2SSampler  *m_input;
    PDMMic  *m_input;
    Transport   *m_transport;
    OutputBuffer *m_output_buffer;

public:
    Application();
    void begin();
    void loop();
};
