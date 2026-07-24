#ifndef IAUDIO_RECORDER_H
#define IAUDIO_RECORDER_H

#include <Arduino.h>

class IAudioRecorder
{
public:
    virtual ~IAudioRecorder() = default;

    virtual void begin() = 0;
    
    // Bắt đầu ghi âm (khởi tạo bộ đệm, bắt đầu đọc I2S)
    virtual void startRecording() = 0;
    
    // Dừng ghi âm và trả về kích thước dữ liệu đã ghi
    virtual void stopRecording() = 0;
    
    // Trả về con trỏ tới bộ đệm chứa dữ liệu PCM
    virtual const uint8_t* getAudioBuffer() const = 0;
    
    // Trả về kích thước dữ liệu thực tế
    virtual size_t getAudioBufferSize() const = 0;
};

#endif // IAUDIO_RECORDER_H
