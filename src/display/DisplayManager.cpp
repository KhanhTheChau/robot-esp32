#include "DisplayManager.h"
#include "../config/AppConfig.h"
#include <Wire.h>

DisplayManager::DisplayManager(ILogger& logger)
    : logger(logger), display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET)
{
}

void DisplayManager::begin()
{
    logger.info("DisplayManager::begin()");
    
    // Khởi tạo I2C với các chân cụ thể cho ESP32-S3
    Wire.begin(AppConfig::I2C_SDA_PIN, AppConfig::I2C_SCL_PIN);
    
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS))
    {
        logger.error("SSD1306 allocation failed");
        return; // Mạch vẫn nên chạy tiếp thay vì crash
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("ESP32 Audio POC"));
    display.display();
}

void DisplayManager::clear()
{
    display.clearDisplay();
}

void DisplayManager::printText(const char* text, int x, int y)
{
    display.setCursor(x, y);
    display.println(text);
}

void DisplayManager::showStatus(const char* status)
{
    clear();
    display.setCursor(0, 0);
    display.println(F("Status:"));
    display.println(status);
    update();
}

void DisplayManager::update()
{
    display.display();
}

