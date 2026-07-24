#ifndef AUDIO_RECORDER_H
#define AUDIO_RECORDER_H

#include "IAudioRecorder.h"
#include "../core/ILogger.h"
#include <driver/i2s.h>
#include <vector>

class AudioRecorder final : public IAudioRecorder
{
public:
    explicit AudioRecorder(ILogger& logger);
    ~AudioRecorder() override;

    void begin() override;
    void startRecording() override;
    void stopRecording() override;
    
    const uint8_t* getAudioBuffer() const override;
    size_t getAudioBufferSize() const override;

private:
    ILogger& logger;
    
    static constexpr i2s_port_t I2S_PORT = I2S_NUM_0;
    static constexpr size_t MAX_AUDIO_BUFFER_SIZE = 16000 * 2 * 3; // 3 giây âm thanh 16kHz 16bit

    std::vector<uint8_t> audioBuffer;
    size_t recordedSize;
    bool isRecording;

    void initI2S();
    void recordTask();
    
    static void i2sTaskHelper(void* pvParameters);
};

#endif // AUDIO_RECORDER_H
