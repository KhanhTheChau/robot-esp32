#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "IDisplay.h"
#include "../core/ILogger.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RoboEyes.h"

class DisplayManager final : public IDisplay
{
public:
    explicit DisplayManager(ILogger& logger);
    ~DisplayManager() override = default;

    void begin() override;
    void clear() override;
    void printText(const char* text, int x = 0, int y = 0) override;
    void showStatus(const char* status) override;
    void drawFace(const String& emotion) override;
    void playGifFrame() override;
    void update() override;
    void setSpeaking(bool isSpeaking) override;

private:
    ILogger& logger;
    Adafruit_SSD1306 display;
    RoboEyes<Adafruit_SSD1306> eyes;
    bool isShowingFace;

    static constexpr uint8_t SCREEN_WIDTH = 128;
    static constexpr uint8_t SCREEN_HEIGHT = 64;
    static constexpr int OLED_RESET = -1;
    static constexpr uint8_t OLED_ADDRESS = 0x3C;
};

#endif // DISPLAY_MANAGER_H
