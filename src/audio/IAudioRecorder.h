#ifndef IAUDIO_RECORDER_H
#define IAUDIO_RECORDER_H

#include <Arduino.h>

class IAudioRecorder
{
public:
    virtual ~IAudioRecorder() = default;

    virtual void begin() = 0;
    
    // Đọc non-blocking, trả về số byte đọc được
    virtual size_t readAudioChunk(uint8_t* buffer, size_t maxLen) = 0;
    
    // Tính toán RMS
    virtual float calculateRMS(const uint8_t* buffer, size_t len) = 0;
    
    virtual void pause() = 0;
    virtual void resume() = 0;
};

#endif // IAUDIO_RECORDER_H
