#ifndef IAUDIO_PLAYER_H
#define IAUDIO_PLAYER_H

#include <Arduino.h>

class IAudioPlayer
{
public:
    virtual ~IAudioPlayer() = default;

    virtual void begin() = 0;
    virtual void write(const uint8_t* data, size_t size) = 0;
};

#endif // IAUDIO_PLAYER_H
