/*
 * Speaker.cpp
 * Copyright (C) 2023 MATSUOKA Takashi <matsujirushi@live.jp>
 * MIT License
 */

////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Speaker.hpp"
#include <Arduino.h>

////////////////////////////////////////////////////////////////////////////////
// Speaker

Speaker* Speaker::instance_ = nullptr;

void Speaker::timerIsr()
{
    if (instance_ == nullptr) abort();

    if (instance_->wavePtr_ != instance_->waveEnd_)
    {
        ledcWrite(instance_->ledControlChannel_, *instance_->wavePtr_++);
    }
    else
    {
        // instance_->stop();
        timerAlarmDisable(instance_->timer_);
    }
}

Speaker::Speaker(int speakerPin, int speakerEnablePin, int ledControlChannel) :
    speakerPin_{ speakerPin },
    speakerEnablePin_{ speakerEnablePin },
    ledControlChannel_{ ledControlChannel },
    timer_{ nullptr },
    wavePtr_{ nullptr },
    waveEnd_{ nullptr }
{
    if (instance_ != nullptr) abort();
    instance_ = this;
}

void Speaker::begin()
{
    if (speakerEnablePin_ >= 0)
    {
        pinMode(speakerEnablePin_, OUTPUT);
        digitalWrite(speakerEnablePin_, HIGH);
    }

    ledcAttachPin(speakerPin_, ledControlChannel_);
    const uint32_t frequency = ledcSetup(ledControlChannel_, SPEAKER_PWM_FREQUENCY, SPEAKER_PWM_RESOLUTION_BITS);
    if (frequency == 0) abort();

    timer_ = timerBegin(0, getApbFrequency() / SPEAKER_TIMER_FREQUENCY, true);
    timerAttachInterrupt(timer_, timerIsr, true);
}

void Speaker::setPowerSupply(bool on)
{
    if (speakerEnablePin_ < 0) return;

    digitalWrite(speakerEnablePin_, on ? LOW : HIGH);
}

void Speaker::setTone(int frequency)
{
    ledcWriteTone(ledControlChannel_, frequency);
}

void Speaker::play(const uint8_t* wave, size_t length, uint32_t frequency)
//void Speaker::play(const uint16_t* wave, size_t length, uint32_t frequency)
{
	noInterrupts();
    wavePtr_ = wave;
    waveEnd_ = wave + length;

    timerAlarmWrite(timer_, SPEAKER_TIMER_FREQUENCY / frequency, true);
    timerAlarmEnable(timer_);
	interrupts();
}

void Speaker::stop()
{
	noInterrupts();
	wavePtr_ = waveEnd_;
	interrupts();
}

bool Speaker::busy()
{
	noInterrupts();
    bool ret = wavePtr_ != waveEnd_;
	interrupts();
	return ret;
}
////////////////////////////////////////////////////////////////////////////////
