#include "VoiceService.h"
#include <Arduino.h>

VoiceService::VoiceService(ILogger& logger, IAudioRecorder& recorder, IAudioPlayer& player, IHttpClient& httpClient)
    : logger(logger), recorder(recorder), player(player), httpClient(httpClient)
{
}

void VoiceService::begin()
{
    logger.info("VoiceService::begin()");
    recorder.begin();
    player.begin();
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
        if (result.text.length() == 0) {
            result.text = "Upload failed";
        }
    }

    return result;
}

int VoiceService::playResponse(const String& url)
{
    logger.info("Playing response audio...");
    if (url.isEmpty())
    {
        logger.error("Audio URL is empty!");
        return 0;
    }

    int totalBytes = 0;
    bool success = httpClient.downloadAudioStream(url, [this, &totalBytes](const uint8_t* data, size_t size) {
        player.write(data, size);
        totalBytes += size;
    });

    if (success) {
        logger.info("Finished playing audio.");
    } else {
        logger.error("Failed to play audio.");
    }
    return totalBytes;
}

