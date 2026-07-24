#include "VoiceService.h"
#include <Arduino.h>

VoiceService::VoiceService(ILogger& logger, IAudioRecorder& recorder, IHttpClient& httpClient)
    : logger(logger), recorder(recorder), httpClient(httpClient)
{
}

void VoiceService::begin()
{
    logger.info("VoiceService::begin()");
}

VoiceResult VoiceService::processVoice()
{
    VoiceResult result;
    
    logger.info("Starting voice processing...");
    
    recorder.startRecording();
    
    // Thu âm 3 giây (hoặc có thể để UI hiển thị và dừng sau). Ở POC ta delay cứng 3 giây để test.
    delay(3000); 
    
    recorder.stopRecording();
    
    size_t dataSize = recorder.getAudioBufferSize();
    const uint8_t* audioData = recorder.getAudioBuffer();
    
    if (dataSize == 0 || audioData == nullptr)
    {
        logger.error("No audio data recorded.");
        return result;
    }

    UploadResult uploadStatus = httpClient.sendAudio(audioData, dataSize, result);
    
    if (uploadStatus != UploadResult::Success)
    {
        logger.error("Failed to upload audio.");
        result.success = false;
        result.text = "Upload failed";
    }

    return result;
}
