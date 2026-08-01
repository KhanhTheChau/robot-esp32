#include "WebSocketManager.h"
#include "../config/AppConfig.h"
#include <ArduinoJson.h>

WebSocketManager::WebSocketManager(ILogger& logger, IWiFiManager& wifi)
    : logger(logger), wifi(wifi)
{
}

void WebSocketManager::begin()
{
    logger.info("WebSocketManager::begin()");
    
    webSocket.begin(AppConfig::WS_API_IP, AppConfig::WS_API_PORT, "/");
    
    webSocket.onEvent([this](WStype_t type, uint8_t * payload, size_t length) {
        this->webSocketEvent(type, payload, length);
    });
    
    webSocket.setReconnectInterval(5000);
}

void WebSocketManager::loop()
{
    webSocket.loop();
}

bool WebSocketManager::isConnected()
{
    return _connected;
}

void WebSocketManager::sendAudioChunk(const uint8_t* data, size_t size)
{
    if (_connected) {
        webSocket.sendBIN(data, size);
    }
}

void WebSocketManager::sendText(const String& text)
{
    if (_connected) {
        webSocket.sendTXT(text.c_str(), text.length());
    }
}

void WebSocketManager::onVoiceResult(std::function<void(const VoiceResult&)> callback)
{
    voiceResultCb = callback;
}

void WebSocketManager::onAudioStream(std::function<void(const uint8_t*, size_t)> callback)
{
    audioStreamCb = callback;
}

void WebSocketManager::webSocketEvent(WStype_t type, uint8_t * payload, size_t length)
{
    switch(type) {
        case WStype_DISCONNECTED:
            logger.info("[WS] Disconnected!");
            _connected = false;
            break;
        case WStype_CONNECTED:
            logger.info("[WS] Connected to Server!");
            _connected = true;
            break;
        case WStype_TEXT:
        {
            logger.info("[WS] Received JSON result");
            if (voiceResultCb) {
                DynamicJsonDocument doc(1024);
                DeserializationError error = deserializeJson(doc, payload);
                if (!error) {
                    VoiceResult result;
                    result.success = doc["success"] | false;
                    result.text = doc.containsKey("text") ? doc["text"].as<const char*>() : "";
                    result.intent = doc.containsKey("intent") ? doc["intent"].as<const char*>() : "";
                    result.confidence = doc["confidence"] | 0.0f;
                    result.audioUrl = doc.containsKey("audio_url") ? doc["audio_url"].as<const char*>() : "";
                    result.emotion = doc.containsKey("emotion") ? doc["emotion"].as<const char*>() : "neutral";
                    voiceResultCb(result);
                } else {
                    logger.error("JSON parse error");
                }
            }
            break;
        }
        case WStype_BIN:
            // Receive TTS stream
            if (audioStreamCb) {
                audioStreamCb(payload, length);
            }
            break;
        default:
            break;
    }
}
