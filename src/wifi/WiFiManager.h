#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include "IWiFiManager.h"
#include "../core/ILogger.h"

class WiFiManager final : public IWiFiManager
{
public:
    explicit WiFiManager(ILogger& logger);
    ~WiFiManager() override = default;

    void begin() override;
    void loop() override;
    bool isConnected() const override;
    int getRSSI() const override;
    const char* getIPAddress() const override;

private:
    ILogger& logger;
    unsigned long reconnectTimer;
    static constexpr unsigned long RECONNECT_INTERVAL_MS = 5000;
    
    mutable char ipBuffer[16];

    void connect();
};

#endif // WIFI_MANAGER_H