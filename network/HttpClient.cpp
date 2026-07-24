#include "HttpClient.h"
#include "../config/AppConfig.h"

HttpClient::HttpClient(ILogger& logger, IWiFiManager& wifi)
    : logger(logger), wifi(wifi)
{
}

UploadResult HttpClient::sendAudio(const uint8_t* audioData, size_t dataSize, VoiceResult& outResult)
{
    if (!wifi.isConnected())
    {
        logger.error("WiFi not connected. Cannot send audio.");
        return UploadResult::ConnectionFailed;
    }

    if (audioData == nullptr || dataSize == 0)
    {
        logger.error("Audio data is empty.");
        return UploadResult::InvalidResponse;
    }

    logger.info("Sending audio to server...");
    
    ::HTTPClient http; // Sử dụng :: để chỉ định class HTTPClient của ESP32, tránh nhầm với class của chúng ta (dù đã ở các namespace/khác tên nhưng cho chắc chắn)
    
    // Lưu ý: thư viện ESP32 là HTTPClient, lớp của chúng ta cũng là HttpClient. C++ phân biệt hoa thường nên không trùng.
    
    http.begin(AppConfig::VOICE_API_URL);
    http.addHeader("Content-Type", "application/octet-stream"); // Gửi dạng raw PCM
    
    int httpResponseCode = http.POST(const_cast<uint8_t*>(audioData), dataSize);
    UploadResult result = UploadResult::Success;

    if (httpResponseCode > 0)
    {
        if (httpResponseCode == 200)
        {
            String payload = http.getString();
            parseResponse(payload, outResult);
        }
        else
        {
            logger.error("HTTP POST failed with code:");
            logger.error(String(httpResponseCode).c_str());
            result = UploadResult::ServerError;
        }
    }
    else
    {
        logger.error("HTTP POST error:");
        logger.error(http.errorToString(httpResponseCode).c_str());
        
        if (httpResponseCode == HTTPC_ERROR_CONNECTION_REFUSED)
            result = UploadResult::ConnectionFailed;
        else
            result = UploadResult::Timeout;
    }

    http.end();
    return result;
}

void HttpClient::parseResponse(const String& payload, VoiceResult& outResult)
{
    logger.debug("Parsing JSON payload...");
    
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
        logger.error("deserializeJson() failed:");
        logger.error(error.c_str());
        outResult.success = false;
        return;
    }

    outResult.success = doc["success"] | false;
    outResult.text = doc["text"] | "";
    outResult.intent = doc["intent"] | "";
    outResult.confidence = doc["confidence"] | 0.0f;
    
    logger.info("Parsed VoiceResult successfully.");
}
