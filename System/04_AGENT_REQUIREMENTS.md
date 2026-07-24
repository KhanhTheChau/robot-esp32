# ESP32 Voice Client Framework - Development Requirements

## Project Goal

Build a production-quality ESP32 application that demonstrates the following workflow:

1. ESP32 boots.
2. Connects to WiFi.
3. Initializes OLED display.
4. Waits for user interaction (button press).
5. Records audio from the microphone.
6. Sends the recorded audio to a remote HTTP server.
7. Receives the server response.
8. Displays the result on the OLED.
9. Prints detailed logs to Serial.

Only implement the components required for this workflow.

Future features such as MQTT, OTA, Firebase, BLE, Sensors, Camera, etc. are **NOT** implemented now, but the architecture must allow them to be added later without major refactoring.

---

# Development Philosophy

This project is **not an Arduino demo**.

It should be designed as a reusable framework for future ESP32 projects.

The implementation must follow:

- Object-Oriented Programming
- SOLID Principles
- Clean Architecture
- Dependency Injection
- Interface-based design

The code must remain easy to extend.

---

# Current Scope

Implement only these modules:

- Application
- Logger
- WiFi Manager
- OLED Display Manager
- Button Manager
- Audio Recorder
- HTTP Client
- Voice Service

Do NOT implement:

- MQTT
- BLE
- OTA
- Firebase
- Camera
- SD Card
- Preferences
- WebSocket

Design the architecture so they can be added later.

---

# Target Workflow

```
Power On
      │
      ▼
Initialize Logger
      │
      ▼
Connect WiFi
      │
      ▼
Initialize OLED
      │
      ▼
Initialize Microphone
      │
      ▼
Waiting Button
      │
Button Pressed
      │
      ▼
Record Audio
      │
      ▼
HTTP POST
      │
      ▼
Receive Response
      │
      ▼
Display Result
      │
      ▼
Waiting Button
```

---

# Required Folder Structure

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

button/
    IButton.h
    ButtonManager.h
    ButtonManager.cpp

audio/
    IAudioRecorder.h
    AudioRecorder.h
    AudioRecorder.cpp

network/
    IHttpClient.h
    HttpClient.h
    HttpClient.cpp

services/
    VoiceService.h
    VoiceService.cpp

models/

utils/
```

---

# Responsibilities

## Application

Only coordinates modules.

Never directly communicates with hardware.

Never calls WiFi.begin().

Never calls HTTPClient.

Never accesses OLED directly.

---

## Logger

The ONLY module allowed to use:

```
Serial.print()
Serial.println()
```

Every other module must use ILogger.

---

## WiFiManager

Responsibilities:

- connect
- reconnect
- status
- IP address
- RSSI

No HTTP logic.

---

## DisplayManager

Responsibilities:

- initialize OLED
- clear screen
- draw text
- draw status

No WiFi logic.

---

## ButtonManager

Responsibilities:

- detect button press
- debounce

No business logic.

---

## AudioRecorder

Responsibilities:

- initialize microphone
- start recording
- stop recording
- return audio buffer

No HTTP logic.

---

## HttpClient

Responsibilities:

- GET
- POST
- multipart upload
- timeout
- retry

No microphone logic.

---

## VoiceService

Coordinates:

AudioRecorder

+

HttpClient

Workflow:

Record audio

↓

Create HTTP request

↓

Upload audio

↓

Receive response

↓

Return parsed result

VoiceService should expose only high-level APIs such as:

```
startRecording()

stopRecording()

sendAudio()

process()
```

Application should never build HTTP requests directly.

---

# Dependency Injection

All dependencies must be passed through constructors.

Correct:

```cpp
VoiceService(
    ILogger& logger,
    IHttpClient& client,
    IAudioRecorder& recorder
);
```

Wrong:

```cpp
VoiceService()
{
    HttpClient client;
}
```

Never instantiate dependencies inside classes.

---

# Coding Rules

Do not use:

- global variables
- singleton
- malloc/free
- new/delete
- delay() for business logic
- String in framework APIs

Prefer:

- constexpr
- enum class
- const correctness
- references
- RAII

---

# Error Handling

Do not return int.

Use strongly typed enums.

Example:

```cpp
enum class UploadResult
{
    Success,
    Timeout,
    ConnectionFailed,
    ServerError,
    InvalidResponse
};
```

---

# Build Requirement

After each module is completed:

- Project must compile.
- No TODOs.
- No placeholder implementations.
- No pseudo code.

Every generated file should be production-ready.