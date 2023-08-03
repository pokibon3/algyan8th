/*
 * mic.hpp
 * Copyright (C) 2023 K.Ohe
 * MIT License
 */

#pragma once

#include <cstdint>

// esp32-hal-timer.h

class PDMMic
{
private:
    int16_t offset;
public:
    PDMMic(void);
    void start();
    void stop();
    void read(uint8_t *buf, size_t bufSize, size_t *readSize);
};
