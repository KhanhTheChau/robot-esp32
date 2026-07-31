#include "DisplayManager.h"
#include "../config/AppConfig.h"
#include <Wire.h>
#include "GifFrames.h"

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

void DisplayManager::drawFace(const String& emotion)
{
    clear();
    
    // Tọa độ tâm màn hình: 64, 32
    if (emotion == "happy")
    {
        display.drawCircleHelper(32, 28, 12, 1, SSD1306_WHITE); 
        display.drawCircleHelper(96, 28, 12, 1, SSD1306_WHITE);
        display.drawCircleHelper(64, 36, 16, 2, SSD1306_WHITE);
    }
    else if (emotion == "sad")
    {
        display.drawLine(20, 16, 44, 24, SSD1306_WHITE);
        display.drawLine(84, 24, 108, 16, SSD1306_WHITE);
        display.drawCircleHelper(64, 56, 16, 1, SSD1306_WHITE);
    }
    else if (emotion == "angry")
    {
        display.drawLine(20, 12, 44, 24, SSD1306_WHITE);
        display.drawLine(84, 24, 108, 12, SSD1306_WHITE);
        display.fillCircle(32, 28, 6, SSD1306_WHITE);
        display.fillCircle(96, 28, 6, SSD1306_WHITE);
        display.drawLine(48, 48, 80, 48, SSD1306_WHITE);
    }
    else if (emotion == "surprised")
    {
        display.drawCircle(32, 24, 10, SSD1306_WHITE);
        display.drawCircle(96, 24, 10, SSD1306_WHITE);
        display.drawCircle(64, 48, 8, SSD1306_WHITE);
    }
    else // neutral
    {
        display.fillCircle(32, 24, 6, SSD1306_WHITE);
        display.fillCircle(96, 24, 6, SSD1306_WHITE);
        display.drawLine(54, 48, 74, 48, SSD1306_WHITE);
    }
    
    update();
}

void DisplayManager::playGifFrame()
{
    static int currentFrame = 0;
    static unsigned long lastFrameTime = 0;
    
    // Phát ở tốc độ ~20fps (50ms mỗi frame)
    if (millis() - lastFrameTime > 50) 
    {
        lastFrameTime = millis();
        clear();
        // Lấy frame hiện tại từ mảng PROGMEM và hiển thị
        display.drawBitmap(0, 0, gif_frames[currentFrame], 128, 64, SSD1306_WHITE);
        update();
        
        currentFrame++;
        if (currentFrame >= gif_frame_count) {
            currentFrame = 0;
        }
    }
}

void DisplayManager::update()
{
    display.display();
}

