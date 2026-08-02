#include "DisplayManager.h"
#include "../config/AppConfig.h"
#include <Wire.h>
// #include "GifFrames.h" // Removed as we use RoboEyes for idle now

DisplayManager::DisplayManager(ILogger& logger)
    : logger(logger), display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET), eyes(display), isShowingFace(false)
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
    
    // Khởi tạo RoboEyes
    eyes.begin(SCREEN_WIDTH, SCREEN_HEIGHT, 50); // 50 fps max
    eyes.setAutoblinker(ON, 3, 2);
    eyes.setIdleMode(ON, 2, 2);
}

void DisplayManager::clear()
{
    isShowingFace = false;
    display.clearDisplay();
}

void DisplayManager::printText(const char* text, int x, int y)
{
    isShowingFace = false;
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

void DisplayManager::drawFace(const String& emotion)
{
    isShowingFace = true;
    
    if (emotion == "happy")
    {
        eyes.setMood(HAPPY);
    }
    else if (emotion == "sad")
    {
        eyes.setMood(TIRED); // RoboEyes dùng TIRED cho buồn/mệt
    }
    else if (emotion == "angry")
    {
        eyes.setMood(ANGRY);
    }
    else if (emotion == "surprised")
    {
        eyes.setMood(DEFAULT);
        eyes.setCuriosity(ON); // Curious mode làm mắt to ra giống ngạc nhiên
    }
    else // neutral
    {
        eyes.setMood(DEFAULT);
        eyes.setCuriosity(OFF);
    }
    
    eyes.update();
}

void DisplayManager::playGifFrame()
{
    if (!isShowingFace) {
        isShowingFace = true;
        eyes.setMood(DEFAULT);
        eyes.setCuriosity(OFF);
    }
    eyes.update();
}

void DisplayManager::update()
{
    if (isShowingFace) {
        eyes.update();
    } else {
        display.display();
    }
}

