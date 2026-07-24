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
    IHttpClient.h
    HttpClient.h
    HttpClient.cpp

button/
    IButton.h
    ButtonManager.h
    ButtonManager.cpp

sensor/
    ISensor.h

utils/

models/

services/
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