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

    // Cấu hình I2S Speaker (MAX98357) cho ESP32-S3
    constexpr uint8_t I2S_SPK_BCLK_PIN = 4;
    constexpr uint8_t I2S_SPK_LRC_PIN  = 5;
    constexpr uint8_t I2S_SPK_DIN_PIN  = 6;

    // Cấu hình I2C cho OLED SSD1306 trên ESP32-S3
    constexpr uint8_t I2C_SDA_PIN = 8;
    constexpr uint8_t I2C_SCL_PIN = 9;

    // Cấu hình Serial
    constexpr unsigned long SERIAL_BAUD_RATE = 115200;

    // Server WebSocket
    constexpr const char* WS_API_IP = "192.168.1.3";
    constexpr uint16_t WS_API_PORT = 5000;

    // Cấu hình VAD
    constexpr float VAD_RMS_THRESHOLD = 200.0f; // Ngưỡng kích hoạt giọng nói (giảm xuống để nhạy hơn với giọng nói nhỏ)
    constexpr unsigned long VAD_SILENCE_TIMEOUT_MS = 2000; // Im lặng 2.0s là hết câu (tăng lên để chờ người lớn tuổi nói chậm)
    constexpr int VAD_CHUNK_SIZE = 1024; // Kích thước mỗi chunk PCM gửi đi
}

#endif // APP_CONFIG_H
