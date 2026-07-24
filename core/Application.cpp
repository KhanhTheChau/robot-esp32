#include "Application.h"
#include <Arduino.h>

Application::Application(
    ILogger& logger,
    IWiFiManager& wifi,
    IDisplay& display,
    IButton& button,
    VoiceService& voice)
    : logger(logger), wifi(wifi), display(display), button(button), voice(voice)
{
}

void Application::begin()
{
    // Thứ tự khởi tạo rất quan trọng
    logger.begin();
    logger.info("Application starting...");

    display.begin();
    display.showStatus("Init WiFi...");

    wifi.begin();
    button.begin();
    voice.begin();

    display.showStatus("Ready.");
}

void Application::loop()
{
    // Duy trì các task background
    wifi.loop();
    button.loop();

    // Logic chính
    if (button.isPressed())
    {
        logger.info("Button pressed, starting voice flow...");
        display.showStatus("Recording...");
        
        VoiceResult result = voice.processVoice();
        
        if (result.success)
        {
            logger.info("Voice process success.");
            logger.info(result.text.c_str());
            logger.info(result.intent.c_str());
            
            display.clear();
            display.printText("Result:", 0, 0);
            display.printText(result.text.c_str(), 0, 10);
            display.printText(result.intent.c_str(), 0, 20);
            display.update();
        }
        else
        {
            logger.error("Voice process failed.");
            display.showStatus("Failed!");
        }
    }
}