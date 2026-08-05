#ifndef AUDIO_RECORDER_H
#define AUDIO_RECORDER_H

#include "IAudioRecorder.h"
#include "../core/ILogger.h"
#include <driver/i2s.h>

class AudioRecorder final : public IAudioRecorder
{
public:
    explicit AudioRecorder(ILogger& logger);
    ~AudioRecorder() override;

    void begin() override;
    
    size_t readAudioChunk(uint8_t* buffer, size_t maxLen) override;
    float calculateRMS(const uint8_t* buffer, size_t len) override;
    
    void pause() override;
    void resume() override;

private:
    ILogger& logger;
    
    static constexpr i2s_port_t I2S_PORT = I2S_NUM_0;

    void initI2S();
    bool _isPaused = false;
};

#endif // AUDIO_RECORDER_H
