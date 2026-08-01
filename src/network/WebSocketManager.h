#ifndef WEBSOCKET_MANAGER_H
#define WEBSOCKET_MANAGER_H

#include "IWebSocketClient.h"
#include "../core/ILogger.h"
#include "../wifi/IWiFiManager.h"
#include <WebSocketsClient.h>

class WebSocketManager final : public IWebSocketClient
{
public:
    WebSocketManager(ILogger& logger, IWiFiManager& wifi);
    
    void begin() override;
    void loop() override;
    bool isConnected() override;
    
    void sendAudioChunk(const uint8_t* data, size_t size) override;
    void sendText(const String& text) override;

    void onVoiceResult(std::function<void(const VoiceResult&)> callback) override;
    void onAudioStream(std::function<void(const uint8_t*, size_t)> callback) override;

private:
    ILogger& logger;
    IWiFiManager& wifi;
    WebSocketsClient webSocket;
    bool _connected = false;
    
    std::function<void(const VoiceResult&)> voiceResultCb;
    std::function<void(const uint8_t*, size_t)> audioStreamCb;

    void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
};

#endif // WEBSOCKET_MANAGER_H
