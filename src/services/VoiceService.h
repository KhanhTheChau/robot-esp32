#ifndef VOICE_SERVICE_H
#define VOICE_SERVICE_H

#include "../core/ILogger.h"
#include "../audio/IAudioRecorder.h"
#include "../audio/IAudioPlayer.h"
#include "../network/IWebSocketClient.h"

enum class VoiceState {
    SILENT,
    SPEAKING
};

class VoiceService
{
public:
    VoiceService(ILogger& logger, IAudioRecorder& recorder, IAudioPlayer& player, IWebSocketClient& webSocket);

    void begin();
    void loop();

    VoiceState getState() const;

private:
    ILogger& logger;
    IAudioRecorder& recorder;
    IAudioPlayer& player;
    IWebSocketClient& webSocket;

    VoiceState state;
    unsigned long lastSpeechTime;
    uint8_t chunkBuffer[1024];
};

#endif // VOICE_SERVICE_H
