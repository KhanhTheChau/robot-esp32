#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "IHttpClient.h"
#include "../core/ILogger.h"
#include "../wifi/IWiFiManager.h"
#include <HTTPClient.h> // ESP32 HTTPClient
#include <ArduinoJson.h>

class HttpClient final : public IHttpClient
{
public:
    HttpClient(ILogger& logger, IWiFiManager& wifi);
    ~HttpClient() override = default;

    UploadResult sendAudio(const uint8_t* audioData, size_t dataSize, VoiceResult& outResult) override;
    bool downloadAudioStream(const String& url, std::function<void(const uint8_t*, size_t)> onData) override;

private:
    ILogger& logger;
    IWiFiManager& wifi;

    void parseResponse(const String& payload, VoiceResult& outResult);
};

#endif // HTTP_CLIENT_H
