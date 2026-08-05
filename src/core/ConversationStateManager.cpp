#include "ConversationStateManager.h"
#include "../config/AppConfig.h"
#include <Arduino.h>

ConversationStateManager::ConversationStateManager(ILogger& logger, IAudioRecorder& recorder, IAudioPlayer& player, IWebSocketClient& webSocket)
    : logger(logger), recorder(recorder), player(player), webSocket(webSocket), 
      _state(ConversationState::SLEEP), _isSpeaking(false), _isRobotSpeaking(false),
      _lastSpeechTime(0), _lastAudioReceiveTime(0)
{
}

void ConversationStateManager::begin()
{
    logger.info("ConversationStateManager::begin()");
    recorder.begin();
    player.begin();
    
    webSocket.onVoiceResult([this](const VoiceResult& result) {
        this->handleServerAction(result);
    });

    webSocket.onAudioStream([this](const uint8_t* data, size_t size) {
        this->handleAudioStream(data, size);
    });
}

void ConversationStateManager::setVoiceResultCallback(std::function<void(const VoiceResult&)> cb)
{
    _resultCb = cb;
}

void ConversationStateManager::handleServerAction(const VoiceResult& result)
{
    if (result.action == "WAKE_UP") {
        logger.info("[State] Switched to AWAKE");
        _state = ConversationState::AWAKE;
    } 
    else if (result.action == "GO_TO_SLEEP") {
        logger.info("[State] Switched to SLEEP");
        _state = ConversationState::SLEEP;
    }
    else if (result.action == "THINKING") {
        logger.info("[State] Robot is THINKING");
    }
    else if (result.action == "CHAT_RESPONSE") {
        logger.info((String("[State] Robot CHAT: ") + result.text).c_str());
    }
    
    if (_resultCb) {
        _resultCb(result);
    }
}

void ConversationStateManager::handleAudioStream(const uint8_t* data, size_t size)
{
    // Cập nhật cờ và tắt ghi âm
    if (!_isRobotSpeaking) {
        _isRobotSpeaking = true;
        recorder.pause();
        logger.info("Robot started speaking. VAD paused.");
    }
    
    _lastAudioReceiveTime = millis();
    player.write(data, size);
}

void ConversationStateManager::checkRobotSpeakingState()
{
    // Nếu quá 500ms không nhận được luồng audio nào từ server -> Robot đã nói xong
    if (_isRobotSpeaking && (millis() - _lastAudioReceiveTime > 500)) {
        _isRobotSpeaking = false;
        recorder.resume();
        logger.info("Robot finished speaking. VAD resumed.");
    }
}

void ConversationStateManager::loop()
{
    // Luôn kiểm tra trạng thái phát TTS để mở lại mic
    checkRobotSpeakingState();

    // Đọc mic non-blocking, nếu _isRobotSpeaking == true (pause) thì readAudioChunk trả về 0
    size_t bytesRead = recorder.readAudioChunk(_chunkBuffer, AppConfig::VAD_CHUNK_SIZE);
    
    if (bytesRead > 0) {
        float rms = recorder.calculateRMS(_chunkBuffer, bytesRead);
        
        if (rms > AppConfig::VAD_RMS_THRESHOLD) {
            _lastSpeechTime = millis();
            if (!_isSpeaking) {
                logger.info("Voice detected! Started streaming to Server.");
                _isSpeaking = true;
            }
            webSocket.sendAudioChunk(_chunkBuffer, bytesRead);
        } else {
            if (_isSpeaking) {
                webSocket.sendAudioChunk(_chunkBuffer, bytesRead); // Gửi thêm chút đuôi nhiễu
                
                if (millis() - _lastSpeechTime > AppConfig::VAD_SILENCE_TIMEOUT_MS) {
                    logger.info("Silence detected. End of speech.");
                    _isSpeaking = false;
                    webSocket.sendText("{\"action\": \"end_of_speech\"}");
                }
            }
        }
    }
}

ConversationState ConversationStateManager::getState() const
{
    return _state;
}

bool ConversationStateManager::isUserSpeaking() const
{
    return _isSpeaking;
}

bool ConversationStateManager::isRobotSpeaking() const
{
    return _isRobotSpeaking;
}
