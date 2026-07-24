# ESP32 IoT Framework

## Mục tiêu

Xây dựng một framework ESP32 theo hướng Object-Oriented Programming (OOP), có khả năng mở rộng, tái sử dụng và bảo trì lâu dài.

Framework không phải chỉ phục vụ một bài demo mà phải có thể dùng làm nền tảng cho các dự án:

- HTTP REST Client
- MQTT
- WebSocket
- Firebase
- OTA
- BLE
- Sensor
- OLED Display
- SD Card
- Camera

Project phải hướng đến chất lượng production.

---

## Triết lý thiết kế

- Clean Architecture
- SOLID Principles
- Dependency Injection
- Interface-based Design
- Low Coupling
- High Cohesion
- Single Responsibility Principle
- Open/Closed Principle

Không viết code theo kiểu Arduino demo.

Mỗi module phải độc lập.

Không module nào biết implementation của module khác.

Các module chỉ giao tiếp thông qua Interface.

---

## Coding Standard

Ngôn ngữ:

- C++17

Platform

- ESP32 Arduino Framework

IDE

- Arduino IDE
- PlatformIO

Không sử dụng:

- global variable
- macro nếu không thật sự cần
- String trong API framework
- delay() để polling
- blocking code

Ưu tiên:

- constexpr
- enum class
- namespace
- constructor injection
- const correctness
- RAII