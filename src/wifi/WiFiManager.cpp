#include "WiFiManager.h"
#include "../config/Secrets.h"
#include <Arduino.h>

WiFiManager::WiFiManager(ILogger& logger)
    : logger(logger), reconnectTimer(0)
{
    memset(ipBuffer, 0, sizeof(ipBuffer));
}

void WiFiManager::begin()
{
    logger.info("WiFiManager::begin()");
    WiFi.mode(WIFI_STA);
    
    // Thêm các điểm truy cập WiFi từ cấu hình Secrets
    for (size_t i = 0; i < Secrets::WIFI_NETWORKS_COUNT; ++i) {
        wifiMulti.addAP(Secrets::WIFI_NETWORKS[i].ssid, Secrets::WIFI_NETWORKS[i].password);
    }
    
    connect();
}

void WiFiManager::loop()
{
    // WiFiMulti.run() tự xử lý kết nối, nếu mất nó tự connect lại mạng mạnh nhất
    if (wifiMulti.run() != WL_CONNECTED)
    {
        if (millis() - reconnectTimer >= RECONNECT_INTERVAL_MS)
        {
            reconnectTimer = millis();
            logger.warning("WiFi disconnected. Reconnecting via WiFiMulti...");
        }
    }
}

bool WiFiManager::isConnected() const
{
    return WiFi.status() == WL_CONNECTED;
}

int WiFiManager::getRSSI() const
{
    if (isConnected())
    {
        return WiFi.RSSI();
    }
    return 0;
}

const char* WiFiManager::getIPAddress() const
{
    if (isConnected())
    {
        String ipStr = WiFi.localIP().toString();
        strncpy(const_cast<char*>(ipBuffer), ipStr.c_str(), sizeof(ipBuffer) - 1);
        ipBuffer[sizeof(ipBuffer) - 1] = '\0';
        return ipBuffer;
    }
    return "0.0.0.0";
}

void WiFiManager::connect()
{
    logger.info("Connecting to WiFi using WiFiMulti...");
    // Gọi lần đầu để kích hoạt quét và kết nối
    wifiMulti.run();
}