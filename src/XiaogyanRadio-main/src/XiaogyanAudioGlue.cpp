#include <Arduino.h>
#include <elapsedMillis.h>
#include "Xiaogyan.hpp"

static const int BUFF_SIZE = 4096;
static uint32_t s_sample_rate = 16000;

void xiaogyan_audio_set_sample_rate(uint32_t sample_rate)
{
    s_sample_rate = sample_rate;
}

bool xiaogyan_audio_write(uint8_t data, int timeout_msec)
{
    //Serial.println("HOGE!");
    
    static uint8_t buffer1[BUFF_SIZE];
    static uint8_t buffer2[BUFF_SIZE];
    static int index = 0;
    
    if(index < BUFF_SIZE){
        buffer1[index] = data;
        index++;
        if(index >= BUFF_SIZE){
            if(Xiaogyan.speaker.busy()){
                elapsedMillis timeout_timer{ 0 };
                while(true){
                    if(timeout_timer >= timeout_msec){
                        Serial.println("ERROR! busy over");
                        index = 0;
                        return false;
                    }
                    if(Xiaogyan.speaker.busy() == false){
                        break;
                    }
                }
            }
            memcpy(buffer2, buffer1, BUFF_SIZE);
            Xiaogyan.speaker.play(buffer2, BUFF_SIZE, s_sample_rate);
            index = 0;
        }
    }else{
        Serial.println("ERROR! index over");
        index = 0;
        return false;
    }
    return true;
}

void xiaogyan_audio_stop()
{
    Xiaogyan.speaker.stop();
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreaminfo(const char *info){
    Serial.print("streaminfo  ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}
