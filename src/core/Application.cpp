#include "Application.h"
#include <Arduino.h>

Application::Application(
    ILogger& logger,
    IWiFiManager& wifi,
    IDisplay& display,
    IButton& button,
    ConversationStateManager& conversationManager,
    IWebSocketClient& webSocket
) : logger(logger), wifi(wifi), display(display), button(button), conversationManager(conversationManager), webSocket(webSocket), currentState(AppState::IDLE)
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
    conversationManager.begin();

    conversationManager.setVoiceResultCallback([this](const VoiceResult& result) {
        if (result.action == "ERROR") {
            display.clear();
            display.playGifFrame();
            display.setSpeaking(false); // Phải tắt miệng nếu bị ngắt ngang do lỗi
            currentState = AppState::IDLE;
        }
        else if (result.action == "CHAT_RESPONSE" || result.action == "THINKING" || result.action == "WAKE_UP" || result.action == "GO_TO_SLEEP") {
            display.clear();
            if (result.action == "THINKING") {
                display.playGifFrame(); // Bỏ chữ Processing
            } else {
                display.drawFace(result.emotion);
            }
            currentState = AppState::SPEAKING;
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

    bool currentConnected = wifi.isConnected() && webSocket.isConnected();
    static bool wasConnected = false;

    if (currentConnected && !wasConnected) {
        logger.info("System fully connected!");
        display.clear(); // Xóa màn hình để mất chữ Connecting Server
        display.update();
        wasConnected = true;
        
        // Luôn đưa state về IDLE khi kết nối lại, phòng hờ bị kẹt ở PROCESSING do rớt mạng
        currentState = AppState::IDLE;
        
    } else if (!currentConnected && wasConnected) {
        wasConnected = false;
    }

    if (currentConnected)
    {
        switch (currentState)
        {
            case AppState::IDLE:
            {
                // Chỉ hiển thị mặt, bỏ chữ Waiting
                display.playGifFrame();
                
                conversationManager.loop(); // VAD liên tục chạy
                
                if (conversationManager.isUserSpeaking())
                {
                    logger.info("Transitioning to LISTENING...");
                    currentState = AppState::LISTENING;
                    // display.showStatus("Listening...");
                }
                break;
            }

            case AppState::LISTENING:
            {
                // Chỉ hiển thị mặt, bỏ chữ Listening
                display.playGifFrame();

                conversationManager.loop(); // Tiếp tục thu âm và gửi WebSocket
                
                if (!conversationManager.isUserSpeaking())
                {
                    logger.info("Transitioning to PROCESSING...");
                    currentState = AppState::PROCESSING;
                    _processStartTime = millis();
                    // display.showStatus("Processing...");
                }
                break;
            }
            
            case AppState::PROCESSING:
            {
                // Hiển thị mặt lúc chờ đợi (đã bỏ chữ Processing)
                display.playGifFrame();
                
                // Timeout chống kẹt vĩnh viễn nếu Server không bao giờ trả lời
                if (millis() - _processStartTime > 15000) {
                    logger.warning("Processing timeout! Reset to IDLE.");
                    currentState = AppState::IDLE;
                }
                break;
            }

            case AppState::SPEAKING:
            {
                conversationManager.loop(); // Cho phép state manager chạy để checkRobotSpeakingState()

                bool isCurrentlySpeaking = conversationManager.isRobotSpeaking();
                display.setSpeaking(isCurrentlySpeaking);
                display.update(); // Keep RoboEyes animated and mouth moving while speaking
                
                if (!isCurrentlySpeaking) {
                    // Robot đã phát xong âm thanh
                    static unsigned long waitEndTime = 0;
                    if (waitEndTime == 0) {
                        waitEndTime = millis();
                    }
                    if (millis() - waitEndTime > 1000) { // Đợi thêm 1s sau khi nói xong
                        waitEndTime = 0;
                        display.setSpeaking(false);
                        currentState = AppState::IDLE;
                    }
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
