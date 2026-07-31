#include "HttpClient.h"
#include "../config/AppConfig.h"
#include <WiFiClient.h>

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
    http.setTimeout(20000); // Tăng timeout lên 20 giây để chờ Gemini suy nghĩ và TTS
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
            outResult.text = "HTTP Code: " + String(httpResponseCode);
            result = UploadResult::ServerError;
        }
    }
    else
    {
        logger.error("HTTP POST error:");
        logger.error(http.errorToString(httpResponseCode).c_str());
        outResult.text = http.errorToString(httpResponseCode);
        
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
    logger.info("Raw JSON payload:");
    logger.info(payload.c_str());
    
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);

    if (error)
    {
        logger.error("deserializeJson() failed:");
        logger.error(error.c_str());
        outResult.success = false;
        return;
    }

    outResult.success = doc["success"] | false;
    outResult.text = doc.containsKey("text") ? doc["text"].as<const char*>() : "";
    outResult.intent = doc.containsKey("intent") ? doc["intent"].as<const char*>() : "";
    outResult.confidence = doc["confidence"] | 0.0f;
    outResult.audioUrl = doc.containsKey("audio_url") ? doc["audio_url"].as<const char*>() : "";
    
    logger.info("Parsed VoiceResult successfully.");
}

bool HttpClient::downloadAudioStream(const String& url, std::function<void(const uint8_t*, size_t)> onData)
{
    if (!wifi.isConnected())
    {
        logger.error("WiFi not connected. Cannot download audio.");
        return false;
    }

    logger.info("Downloading audio from:");
    logger.info(url.c_str());

    ::HTTPClient http;
    http.begin(url);
    
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        int len = http.getSize();
        WiFiClient* stream = http.getStreamPtr();
        
        uint8_t buff[512] = { 0 };
        while (http.connected() && (len > 0 || len == -1))
        {
            size_t size = stream->available();
            if (size)
            {
                int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                if (c > 0 && onData)
                {
                    onData(buff, c);
                }
                if (len > 0) {
                    len -= c;
                }
            }
            delay(1);
        }
        http.end();
        return true;
    }
    else
    {
        logger.error("HTTP GET failed with code:");
        logger.error(String(httpCode).c_str());
    }

    http.end();
    return false;
}
