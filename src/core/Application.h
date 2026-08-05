#ifndef APPLICATION_H
#define APPLICATION_H

#include "ILogger.h"
#include "../wifi/IWiFiManager.h"
#include "../display/IDisplay.h"
#include "../button/IButton.h"
#include "ConversationStateManager.h"
#include "../network/IWebSocketClient.h"

class Application final
{
public:
    Application(
        ILogger& logger,
        IWiFiManager& wifi,
        IDisplay& display,
        IButton& button,
        ConversationStateManager& conversationManager,
        IWebSocketClient& webSocket
    );

    void begin();
    void loop();

private:
    ILogger& logger;
    IWiFiManager& wifi;
    IDisplay& display;
    IButton& button;
    ConversationStateManager& conversationManager;
    IWebSocketClient& webSocket;

    enum class AppState {
        IDLE,
        LISTENING,
        PROCESSING,
        SPEAKING
    };
    AppState currentState;

    void updateDisplayStatus();
};

#endif // APPLICATION_H
