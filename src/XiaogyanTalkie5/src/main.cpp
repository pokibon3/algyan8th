/*
 * main.cpp
 * Copyright (C) 2023 K.Ohe 
 * MIT License
 */
// Includes
#include <Arduino.h>
#include <elapsedMillis.h>
#include "Xiaogyan.hpp"
#include "Application.h"

// Variables
static int EncoderValue_ = 1;
Application *application;

// setup and loop
void setup()
{
    Serial.begin(115200);

    ////////////////////////////////////////
    // Initialize
    Xiaogyan.begin();
    Xiaogyan.encoder.setRotatedHandler([](bool cw){
        const int value = EncoderValue_ + (cw ? -1 : 1);
        EncoderValue_ = constrain(value, 1, 13);
        Serial.println(EncoderValue_);
    });

    // Startup Sequence
    //Xiaogyan.speaker.setTone(262);  // C4
    Xiaogyan.ledMatrix.setBrightness(2);
    Xiaogyan.ledMatrix.fillScreen(1);
    delay(200);
    Xiaogyan.ledMatrix.fillScreen(2);
    delay(200);
    Xiaogyan.ledMatrix.fillScreen(3);
    delay(200);
    //Xiaogyan.speaker.setTone(0);
    Xiaogyan.ledMatrix.fillScreen(0);
    // Start Walkie Talkie
    application = new Application();
    application->begin();
    Serial.println("XIAOGYAN Walkie Talkie Application started");

}

void loop()
{
    // Xiaogyan
    Xiaogyan.doWork();
    // LED
    Xiaogyan.led.write(millis() % 1000 < 200 ? LOW : HIGH);

    vTaskDelay(pdMS_TO_TICKS(1));

}

////////////////////////////////////////////////////////////////////////////////
