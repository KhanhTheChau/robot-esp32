#include "Application.h"
#include <Arduino.h>

Application::Application(
    ILogger& logger,
    IWiFiManager& wifi,
    IDisplay& display,
    IButton& button,
    VoiceService& voice,
    IWebSocketClient& webSocket
) : logger(logger), wifi(wifi), display(display), button(button), voice(voice), webSocket(webSocket), currentState(AppState::IDLE)
{
}

void Application::begin()
{
    logger.info("Application::begin() - Booting ESP32 System");
    display.begin();
    display.showStatus("Booting...");

    button.begin();
    wifi.begin();
    webSocket.begin();
    voice.begin();

    webSocket.onVoiceResult([this](const VoiceResult& result) {
        if (result.success) {
            logger.info("Server responded with AI reply.");
            display.clear();
            display.drawFace(result.emotion);
            currentState = AppState::SPEAKING;
        } else {
            logger.error("Server error.");
            display.clear();
            display.printText("Error from server", 0, 0);
            display.update();
            delay(2000);
            updateDisplayStatus();
            currentState = AppState::IDLE;
        }
    });

    updateDisplayStatus();
}

void Application::updateDisplayStatus()
{
    if (wifi.isConnected())
    {
        display.clear();
        display.update();
    }
    else
    {
        display.showStatus("Connecting WiFi...");
    }
}

void Application::loop()
{
    wifi.loop();
    button.loop();
    webSocket.loop();

    if (wifi.isConnected() && webSocket.isConnected())
    {
        switch (currentState)
        {
            case AppState::IDLE:
            {
                display.playGifFrame();
                voice.loop(); // VAD liên tục chạy
                
                if (voice.getState() == VoiceState::SPEAKING)
                {
                    logger.info("Transitioning to LISTENING...");
                    currentState = AppState::LISTENING;
                    display.showStatus("Listening...");
                }
                break;
            }

            case AppState::LISTENING:
            {
                // Nhấp nháy chữ Listening
                if ((millis() / 500) % 2 == 0) {
                    display.showStatus("Listening [*]");
                } else {
                    display.showStatus("Listening [ ]");
                }

                voice.loop(); // Tiếp tục thu âm và gửi WebSocket
                
                if (voice.getState() == VoiceState::SILENT)
                {
                    logger.info("Transitioning to PROCESSING...");
                    currentState = AppState::PROCESSING;
                    display.showStatus("Processing...");
                }
                break;
            }
            
            case AppState::PROCESSING:
            {
                // Chờ callback onVoiceResult từ WebSocket đổi state
                break;
            }

            case AppState::SPEAKING:
            {
                static unsigned long speakingStartTime = 0;
                if (speakingStartTime == 0) {
                    speakingStartTime = millis();
                }

                // Phát audio đang được xử lý ở nền
                
                if (millis() - speakingStartTime > 8000) { // Đợi 8 giây cho việc đọc text an toàn
                    speakingStartTime = 0;
                    updateDisplayStatus();
                    currentState = AppState::IDLE;
                }
                break;
            }
        }
    }
    else
    {
        // Vẫn hiển thị GIF hoặc Connecting
        if (!wifi.isConnected()) {
            display.showStatus("Connecting WiFi...");
        } else if (!webSocket.isConnected()) {
            display.showStatus("Connecting Server...");
        }
    }
}
