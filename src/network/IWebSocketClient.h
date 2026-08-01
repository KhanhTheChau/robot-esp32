#ifndef IWEBSOCKET_CLIENT_H
#define IWEBSOCKET_CLIENT_H

#include "../models/VoiceResult.h"
#include <functional>

class IWebSocketClient
{
public:
    virtual ~IWebSocketClient() = default;

    virtual void begin() = 0;
    virtual void loop() = 0;
    virtual bool isConnected() = 0;
    
    virtual void sendAudioChunk(const uint8_t* data, size_t size) = 0;
    virtual void sendText(const String& text) = 0;

    virtual void onVoiceResult(std::function<void(const VoiceResult&)> callback) = 0;
    virtual void onAudioStream(std::function<void(const uint8_t*, size_t)> callback) = 0;
};

#endif // IWEBSOCKET_CLIENT_H
