#include "Application.h"
#include <Arduino.h>

Application::Application(
    ILogger& logger,
    IWiFiManager& wifi,
    IDisplay& display,
    IButton& button,
    VoiceService& voice
) : logger(logger), wifi(wifi), display(display), button(button), voice(voice), currentState(AppState::IDLE)
{
}

void Application::begin()
{
    logger.info("Application::begin() - Booting ESP32 System");
    display.begin();
    display.showStatus("Booting...");

    button.begin();
    wifi.begin();
    voice.begin();

    updateDisplayStatus();
}

void Application::updateDisplayStatus()
{
    if (wifi.isConnected())
    {
        display.showStatus("Ready! Hold BOOT");
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

    switch (currentState)
    {
        case AppState::IDLE:
        {
            if (button.isPressed()) 
            {
                logger.info("Button pressed, transitioning to RECORDING...");
                currentState = AppState::RECORDING;
                display.showStatus("Recording...");
                voice.startRecording();
            }
            break;
        }

        case AppState::RECORDING:
        {
            // Trực quan hóa: Nhấp nháy chữ Recording
            if ((millis() / 500) % 2 == 0) {
                display.showStatus("Recording [*]");
            } else {
                display.showStatus("Recording [ ]");
            }

            if (!button.isHeld())
            {
                logger.info("Button released, transitioning to PROCESSING...");
                currentState = AppState::PROCESSING;
                display.showStatus("Uploading...");
                
                // Stop and Upload
                VoiceResult result = voice.stopAndUpload();
                
                display.clear();
                if (result.success)
                {
                    logger.info("Voice processed successfully");
                    
                    // Hiển thị KHUÔN MẶT CẢM XÚC lên OLED
                    display.drawFace(result.emotion);
                    
                    if (result.audioUrl.length() > 0)
                    {
                        // Gọi luồng tải âm thanh
                        voice.playResponse(result.audioUrl);
                        
                        // Đợi 2 giây sau khi phát xong để user nhìn mặt
                        delay(2000);
                    }
                    else
                    {
                        display.clear();
                        display.printText("NO AUDIO LINK!", 0, 0);
                        display.update();
                        delay(2000);
                    }
                }
                else
                {
                    logger.error("Voice processing failed");
                    display.clear();
                    display.printText("Error:", 0, 0);
                    display.printText(result.text.c_str(), 0, 16);
                    display.update();
                }

                delay(3000); // Đợi 3 giây để đọc kết quả
                updateDisplayStatus();
                currentState = AppState::IDLE;
            }
            break;
        }
        
        case AppState::PROCESSING:
            // Không làm gì, đang block trong VoiceService::stopAndUpload()
            break;
    }
}

