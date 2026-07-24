#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <Arduino.h>

namespace AppConfig
{
    // C?u hình Nút nh?n BOOT (thu?ng là GPIO 0, Active LOW)
    constexpr uint8_t BUTTON_PIN = 0;

    // C?u hình I2S Microphone (INMP441, tùy m?ch, ? dây t?m gán các pin I2S m?c d?nh ho?c ph? bi?n)
    constexpr uint8_t I2S_SD_PIN = 32;
    constexpr uint8_t I2S_WS_PIN = 25;
    constexpr uint8_t I2S_SCK_PIN = 33;
    constexpr int I2S_SAMPLE_RATE = 16000;

    // C?u hình Serial
    constexpr unsigned long SERIAL_BAUD_RATE = 115200;

    // API Server url
    constexpr const char* VOICE_API_URL = "http://api.example.com/voice";
}

#endif // APP_CONFIG_H
