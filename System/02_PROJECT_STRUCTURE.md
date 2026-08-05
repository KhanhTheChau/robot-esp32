# Folder Structure

```
ESP32_Project/

main.ino

config/
    AppConfig.h
    Secrets.h

core/
    Application.h
    Application.cpp
    ConversationStateManager.h
    ConversationStateManager.cpp

    ILogger.h
    Logger.h
    Logger.cpp

wifi/
    IWiFiManager.h
    WiFiManager.h
    WiFiManager.cpp

display/
    IDisplay.h
    DisplayManager.h
    DisplayManager.cpp

network/
    IWebSocketClient.h
    WebSocketManager.h
    WebSocketManager.cpp

button/
    IButton.h
    ButtonManager.h
    ButtonManager.cpp

audio/
    IAudioRecorder.h
    AudioRecorder.h
    AudioRecorder.cpp
    IAudioPlayer.h
    AudioPlayer.h
    AudioPlayer.cpp

utils/

models/
    VoiceResult.h
```

---

## Quy tắc

Mỗi class

1 header

1 cpp

Không viết nhiều class trong một file.

Không viết business logic trong header.

Không include thừa.

Dùng forward declaration nếu có thể.