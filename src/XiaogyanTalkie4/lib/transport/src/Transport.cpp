#include "Arduino.h"
#include "Transport.h"

Transport::Transport(OutputBuffer *output_buffer, size_t buffer_size)
{
  m_output_buffer = output_buffer;
  m_buffer_size = buffer_size;
  m_buffer = (uint8_t *)malloc(m_buffer_size);
  m_index = 0;
  m_header_size = 0;
}

void Transport::add_sample(int16_t sample)
{
//  uint8_t work =(sample + 32768UL) >> 8;
//  uint8_t work =(sample + 600UL) >> 3;
  int16_t work = sample >> 3;
          work = (work < -128) ? -128 : ((work > 127) ? 127 : work) ;
  uint8_t work2 = (work + 128) & 0x00ff;

  m_buffer[m_index+m_header_size] = work2;
//  m_buffer[m_index+m_header_size] = sample;
//  Serial.println(work2);
//  Serial.println(sample);
  m_index++;
  // have we reached a full packet?
  if ((m_index + m_header_size) == m_buffer_size)
  {
//    Serial.println("Send");
    send();
    m_index = 0;
  }
}

void Transport::flush()
{
  if (m_index >0 )
  {
    send();
    m_index = 0;
  }
}

int Transport::set_header(const int header_size, const uint8_t *header)
{
  if ((header_size<m_buffer_size) && (header))
  {
    m_header_size = header_size;
    memcpy(m_buffer, header, header_size);
    return 0;
  }
  else
  {
    return -1;
  }
}
