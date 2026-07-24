#include "VoiceService.h"
#include <Arduino.h>

VoiceService::VoiceService(ILogger& logger, IAudioRecorder& recorder, IHttpClient& httpClient)
    : logger(logger), recorder(recorder), httpClient(httpClient)
{
}

void VoiceService::begin()
{
    logger.info("VoiceService::begin()");
    recorder.begin();
}

void VoiceService::startRecording()
{
    logger.info("Starting voice recording...");
    recorder.startRecording();
}

VoiceResult VoiceService::stopAndUpload()
{
    VoiceResult result;
    
    logger.info("Stopping recording and starting upload...");
    recorder.stopRecording();
    
    size_t dataSize = recorder.getAudioBufferSize();
    const uint8_t* audioData = recorder.getAudioBuffer();
    
    if (dataSize == 0 || audioData == nullptr)
    {
        logger.error("No audio data recorded.");
        result.success = false;
        result.text = "No audio";
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

