#include "config/AppConfig.h"
#include "core/Logger.h"
#include "wifi/WiFiManager.h"
#include "display/DisplayManager.h"
#include "button/ButtonManager.h"
#include "audio/AudioRecorder.h"
#include "network/HttpClient.h"
#include "services/VoiceService.h"
#include "core/Application.h"

// 1. Khởi tạo các Core Component
Logger logger;

// 2. Khởi tạo các Manager (truyền logger qua Dependency Injection)
WiFiManager wifiManager(logger);
DisplayManager displayManager(logger);
ButtonManager buttonManager(logger, AppConfig::BUTTON_PIN);
AudioRecorder audioRecorder(logger);
HttpClient httpClient(logger, wifiManager);

// 3. Khởi tạo các Service
VoiceService voiceService(logger, audioRecorder, httpClient);

// 4. Khởi tạo Application (Root của Dependency Injection)
Application app(
    logger,
    wifiManager,
    displayManager,
    buttonManager,
    voiceService
);

void setup()
{
    // Mọi logic khởi tạo nằm bên trong app.begin()
    app.begin();
}

void loop()
{
    // Mọi logic hoạt động nằm bên trong app.loop()
    app.loop();
}