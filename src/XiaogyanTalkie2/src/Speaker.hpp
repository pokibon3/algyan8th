/*
 * Speaker.hpp
 * Copyright (C) 2023 MATSUOKA Takashi <matsujirushi@live.jp>
 * MIT License
 */

#pragma once

#include <cstdbool>
#include <cstddef>
#include <cstdint>

// esp32-hal-timer.h
struct hw_timer_s;
typedef struct hw_timer_s hw_timer_t;

class Speaker
{
private:
    static constexpr uint32_t SPEAKER_PWM_FREQUENCY = 156250;
    static constexpr uint8_t SPEAKER_PWM_RESOLUTION_BITS = 8;
    //static constexpr uint8_t SPEAKER_PWM_RESOLUTION_BITS = 16;
    static constexpr uint32_t SPEAKER_TIMER_FREQUENCY = 10000000;   // 10[MHz]

private:
    static Speaker* instance_;

    int speakerPin_;
    int speakerEnablePin_;
    int ledControlChannel_;

    hw_timer_t* timer_;
    const uint8_t* wavePtr_;
    const uint8_t* waveEnd_;

private:
    static void timerIsr();

public:
    Speaker(int speakerPin, int speakerEnablePin, int ledControlChannel);

    void begin();
    void setPowerSupply(bool on);
    void setTone(int frequency);
    void play(const uint8_t* wave, size_t length, uint32_t frequency);
    void stop();
    bool busy();

};
