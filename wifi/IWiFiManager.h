#ifndef IWIFI_MANAGER_H
#define IWIFI_MANAGER_H

class IWiFiManager
{
public:
    virtual ~IWiFiManager() = default;

    virtual void begin() = 0;
    virtual void loop() = 0;
    virtual bool isConnected() const = 0;
    virtual int getRSSI() const = 0;
    virtual const char* getIPAddress() const = 0;
};

#endif // IWIFI_MANAGER_H
