#include "src/config/AppConfig.h"
#include "src/core/Logger.h"
#include "src/wifi/WiFiManager.h"
#include "src/display/DisplayManager.h"
#include "src/button/ButtonManager.h"
#include "src/audio/AudioRecorder.h"
#include "src/audio/AudioPlayer.h"
#include "src/network/HttpClient.h"
#include "src/services/VoiceService.h"
#include "src/core/Application.h"

// 1. Khá»Ÿi táº¡o cÃ¡c Core Component
Logger logger;

// 2. Khá»Ÿi táº¡o cÃ¡c Manager (truyá» n logger qua Dependency Injection)
WiFiManager wifiManager(logger);
DisplayManager displayManager(logger);
ButtonManager buttonManager(logger, AppConfig::BUTTON_PIN);
AudioRecorder audioRecorder(logger);
AudioPlayer audioPlayer(logger);
HttpClient httpClient(logger, wifiManager);

// 3. Khá»Ÿi táº¡o cÃ¡c Service
VoiceService voiceService(logger, audioRecorder, audioPlayer, httpClient);

// 4. Khá»Ÿi táº¡o Application (Root cá»§a Dependency Injection)
Application app(
    logger,
    wifiManager,
    displayManager,
    buttonManager,
    voiceService
);

void setup()
{
    // Má»i logic khá»Ÿi táº¡o náº±m bÃªn trong app.begin()
    app.begin();
}

void loop()
{
    // Má»i logic hoáº¡t Ä‘á»™ng náº±m bÃªn trong app.loop()
    app.loop();
}
