#include "src/config/AppConfig.h"
#include "src/core/Logger.h"
#include "src/wifi/WiFiManager.h"
#include "src/display/DisplayManager.h"
#include "src/button/ButtonManager.h"
#include "src/audio/AudioRecorder.h"
#include "src/audio/AudioPlayer.h"
#include "src/network/WebSocketManager.h"
#include "src/services/VoiceService.h"
#include "src/core/Application.h"

// 1. Khởi tạo các Core Component
Logger logger;

// 2. Khởi tạo các Manager (truyền logger qua Dependency Injection)
WiFiManager wifiManager(logger);
DisplayManager displayManager(logger);
ButtonManager buttonManager(logger, AppConfig::BUTTON_PIN);
AudioRecorder audioRecorder(logger);
AudioPlayer audioPlayer(logger);
WebSocketManager webSocketManager(logger, wifiManager);

// 3. Khởi tạo các Service
VoiceService voiceService(logger, audioRecorder, audioPlayer, webSocketManager);

// 4. Khởi tạo Application (Root của Dependency Injection)
Application app(
    logger,
    wifiManager,
    displayManager,
    buttonManager,
    voiceService,
    webSocketManager
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
