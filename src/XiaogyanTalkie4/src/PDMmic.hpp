/*
 * mic.hpp
 * Copyright (C) 2023 K.Ohe
 * MIT License
 */

#pragma once

#include <cstdint>

// esp32-hal-timer.h

class PDMmic
{
public:
    void begin();
    void read(uint8_t *buf, size_t bufSize, size_t *readSize);
};
