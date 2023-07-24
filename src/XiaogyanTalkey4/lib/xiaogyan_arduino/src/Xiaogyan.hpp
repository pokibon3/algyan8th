/*
 * Xiaogyan.hpp
 * Copyright (C) 2023 MATSUOKA Takashi <matsujirushi@live.jp>
 * MIT License
 */

#pragma once

#include <cstdint>

#include "internal/Gpio.hpp"

#include "internal/TM1640.hpp"
#include "internal/LedMatrix.hpp"
#include "internal/LedMatrixAGFX.hpp"
#include "internal/Speaker.hpp"
#include "internal/Encoder.hpp"

class XiaogyanClass
{
private:
    static constexpr uint8_t TM1640_DIN_PIN     = D10;
    static constexpr uint8_t TM1640_SCLK_PIN    = D8;
    static constexpr uint8_t SPEAKER_PIN        = D0;
    static constexpr uint8_t SPEAKER_ENABLE_PIN = -1;   // D3
    static constexpr uint8_t ENCODER_A_PIN      = D1;
    static constexpr uint8_t ENCODER_B_PIN      = D2;
    static constexpr uint8_t LED_PIN            = D6;
    static constexpr uint8_t BUTTON_A_PIN       = D9;
    static constexpr uint8_t BUTTON_B_PIN       = D7;

    static constexpr uint8_t SPEAKER_LEDC_CHANNEL = 0;

    static const uint8_t TM1640_GRID_MAP[];
    static const uint8_t TM1640_ROW_MAP[];

private:
    static TM1640 tm1640_;
    static LedMatrix ledMatrix_;

public:
    static LedMatrixAGFX ledMatrix;
    static Speaker speaker;
    static Encoder encoder;
    static GpioOutputPin<LED_PIN> led;
    static GpioInputPin<BUTTON_A_PIN> buttonA;
    static GpioInputPin<BUTTON_B_PIN> buttonB;

public:
    static void begin();
    static void doWork();

};

extern XiaogyanClass Xiaogyan;
