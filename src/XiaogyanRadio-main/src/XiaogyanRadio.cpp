#include <Arduino.h>
#include "Xiaogyan.hpp"
#include "Audio.h"

Audio audio;

// WiFi settings
String ssid =     "bon1";
String password = "mayutaro";

// radio stations
String stations[] ={
        "ais-sa2.cdnstream1.com/b22139_128mp3",   // 101 SMOOTH JAZZ
        "ice1.somafm.com/seventies-128-mp3",      // SomaFM / Left Coast 70s
        "ice1.somafm.com/illstreet-128-mp3",      // SomaFM / Illinois Street Lounge
        "ice1.somafm.com/secretagent-128-mp3",    // SomaFM / Secret Agent
        "ice1.somafm.com/bootliquor-128-mp3",     // SomaFM / Boot Liquor
        "relax.stream.publicradio.org/relax.mp3", // Your Classical - Relax
};
const int stationsMax = 6;

int cur_station  = 0;         // current station No.

// show number on LED matrix
void showNumber(int number) 
{
    const char abc = '0' + number;
    Xiaogyan.ledMatrix.drawChar(
        1, 0,               // position (x,y)
        abc,                // character
        1,                  // color
        0,                  // back ground color
        1);                 // size
}


void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println();
    Serial.println();
    
    Xiaogyan.begin();
	
    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED){
        Serial.println("WiFi connecting...");
        delay(1000);
    }
    Serial.println("WiFi start");

    Xiaogyan.ledMatrix.setBrightness(2);
    Xiaogyan.ledMatrix.fillScreen(0);
    showNumber(1 + cur_station);
    
    audio.setVolume(15); // 0...21　

//  audio.connecttospeech("Wenn die Hunde schlafen, kann der Wolf gut Schafe stehlen.", "de");
//  audio.connecttospeech("When the dogs sleep, the wolf is good at stealing sheep.", "en");

    audio.stopSong();
    audio.connecttohost(stations[cur_station].c_str());
}

void loop()
{
    audio.loop();

    // Buttons
    static bool buttonA = false;
    static bool buttonB = false;
    bool preButtonA = buttonA;
    bool preButtonB = buttonB;
    buttonA = Xiaogyan.buttonA.read() == LOW;
    buttonB = Xiaogyan.buttonB.read() == LOW;
    if ((preButtonA != buttonA) && buttonA)
    {
        cur_station++;
        if(cur_station >= stationsMax) cur_station = 0;
        showNumber(1 + cur_station);
        audio.stopSong();
        audio.connecttohost(stations[cur_station].c_str());
    }
    else if((preButtonB != buttonB) && buttonB)
    {
        cur_station--;
        if(cur_station < 0) cur_station = stationsMax - 1;
        showNumber(1 + cur_station);
        audio.stopSong();
        audio.connecttohost(stations[cur_station].c_str());
    }
}
