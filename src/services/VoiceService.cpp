#include "VoiceService.h"
#include "../config/AppConfig.h"
#include <Arduino.h>

VoiceService::VoiceService(ILogger& logger, IAudioRecorder& recorder, IAudioPlayer& player, IWebSocketClient& webSocket)
    : logger(logger), recorder(recorder), player(player), webSocket(webSocket), state(VoiceState::SILENT), lastSpeechTime(0), lastAudioReceiveTime(0)
{
}

void VoiceService::begin()
{
    logger.info("VoiceService::begin()");
    recorder.begin();
    player.begin();
    
    // Bind WebSocket events
    webSocket.onAudioStream([this](const uint8_t* data, size_t size) {
        this->lastAudioReceiveTime = millis();
        this->player.write(data, size);
    });
}

void VoiceService::loop()
{
    size_t bytesRead = recorder.readAudioChunk(chunkBuffer, AppConfig::VAD_CHUNK_SIZE);
    
    if (bytesRead > 0) {
        float rms = recorder.calculateRMS(chunkBuffer, bytesRead);
        
        static unsigned long lastRmsPrint = 0;
        if (millis() - lastRmsPrint > 1000) {
            String msg = String("Current Audio RMS: ") + String(rms);
            logger.info(msg.c_str());
            lastRmsPrint = millis();
        }
        
        if (rms > AppConfig::VAD_RMS_THRESHOLD) {
            lastSpeechTime = millis();
            if (state == VoiceState::SILENT) {
                logger.info("Voice detected! Started streaming.");
                state = VoiceState::SPEAKING;
            }
            // Gửi chunk audio liên tục
            webSocket.sendAudioChunk(chunkBuffer, bytesRead);
        } else {
            // Im lặng
            if (state == VoiceState::SPEAKING) {
                // Vẫn gửi audio (tiếng ồn nền ngắn)
                webSocket.sendAudioChunk(chunkBuffer, bytesRead);
                
                // Nếu im lặng quá lâu
                if (millis() - lastSpeechTime > AppConfig::VAD_SILENCE_TIMEOUT_MS) {
                    logger.info("Silence detected. End of speech.");
                    state = VoiceState::SILENT;
                    
                    // Gửi cờ chốt luồng
                    webSocket.sendText("{\"action\": \"end_of_speech\"}");
                }
            }
        }
    }
}

VoiceState VoiceService::getState() const
{
    return state;
}

unsigned long VoiceService::getLastAudioReceiveTime() const
{
    return lastAudioReceiveTime;
}
