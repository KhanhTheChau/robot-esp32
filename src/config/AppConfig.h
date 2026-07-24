#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <Arduino.h>

namespace AppConfig
{
    // Cấu hình Nút nhấn BOOT (thường là GPIO 0 trên ESP32-S3)
    constexpr uint8_t BUTTON_PIN = 0;

    // Cấu hình I2S Microphone (INMP441) cho ESP32-S3
    constexpr uint8_t I2S_SCK_PIN = 16; // BCLK
    constexpr uint8_t I2S_WS_PIN  = 17; // LRCLK
    constexpr uint8_t I2S_SD_PIN  = 18; // DOUT
    constexpr int I2S_SAMPLE_RATE = 16000;

    // Cấu hình I2C cho OLED SSD1306 trên ESP32-S3
    constexpr uint8_t I2C_SDA_PIN = 8;
    constexpr uint8_t I2C_SCL_PIN = 9;

    // Cấu hình Serial
    constexpr unsigned long SERIAL_BAUD_RATE = 115200;

    // API Server url (Máy tính chạy server.py phải cùng mạng WiFi với mạch)
    // Sửa YOUR_COMPUTER_IP thành IP LAN của máy tính (ví dụ: 192.168.1.10)
    constexpr const char* VOICE_API_URL = "http://192.168.1.6:5000/upload";
}

#endif // APP_CONFIG_H
