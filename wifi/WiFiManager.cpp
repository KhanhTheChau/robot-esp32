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
    connect();
}

void WiFiManager::loop()
{
    if (!isConnected())
    {
        if (millis() - reconnectTimer >= RECONNECT_INTERVAL_MS)
        {
            reconnectTimer = millis();
            logger.warning("WiFi disconnected. Reconnecting...");
            connect();
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
    logger.info("Connecting to WiFi...");
    logger.debug(Secrets::WIFI_SSID);
    
    // Non-blocking WiFi begin
    WiFi.begin(Secrets::WIFI_SSID, Secrets::WIFI_PASSWORD);
}