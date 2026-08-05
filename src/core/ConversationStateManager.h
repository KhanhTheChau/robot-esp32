#ifndef CONVERSATION_STATE_MANAGER_H
#define CONVERSATION_STATE_MANAGER_H

#include "../core/ILogger.h"
#include "../audio/IAudioRecorder.h"
#include "../audio/IAudioPlayer.h"
#include "../network/IWebSocketClient.h"
#include <functional>

enum class ConversationState {
    SLEEP,
    AWAKE
};

class ConversationStateManager
{
public:
    ConversationStateManager(ILogger& logger, IAudioRecorder& recorder, IAudioPlayer& player, IWebSocketClient& webSocket);

    void begin();
    void loop();

    ConversationState getState() const;
    bool isUserSpeaking() const;
    bool isRobotSpeaking() const;
    void setVoiceResultCallback(std::function<void(const VoiceResult&)> cb);

private:
    ILogger& logger;
    IAudioRecorder& recorder;
    IAudioPlayer& player;
    IWebSocketClient& webSocket;

    ConversationState _state;
    bool _isSpeaking; // Trạng thái người dùng đang nói
    bool _isRobotSpeaking; // Trạng thái robot đang phát tiếng
    
    unsigned long _lastSpeechTime;
    unsigned long _lastAudioReceiveTime;
    uint8_t _chunkBuffer[1024];

    std::function<void(const VoiceResult&)> _resultCb;

    void handleServerAction(const VoiceResult& result);
    void handleAudioStream(const uint8_t* data, size_t size);
    void checkRobotSpeakingState();
};

#endif // CONVERSATION_STATE_MANAGER_H
