#ifndef APPLICATION_H
#define APPLICATION_H

#include "ILogger.h"
#include "../wifi/IWiFiManager.h"
#include "../display/IDisplay.h"
#include "../button/IButton.h"
#include "../services/VoiceService.h"

class Application final
{
public:
    Application(
        ILogger& logger,
        IWiFiManager& wifi,
        IDisplay& display,
        IButton& button,
        VoiceService& voice
    );

    void begin();
    void loop();

private:
    ILogger& logger;
    IWiFiManager& wifi;
    IDisplay& display;
    IButton& button;
    VoiceService& voice;

    enum class AppState {
        IDLE,
        RECORDING,
        PROCESSING
    };
    AppState currentState;

    void updateDisplayStatus();
};

#endif // APPLICATION_H
