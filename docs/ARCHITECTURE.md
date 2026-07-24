# Kiến trúc phần mềm: ESP32 Audio POC

Dự án này được thiết kế và triển khai dựa trên **Clean Architecture**, **SOLID**, và **Dependency Injection (DI)**, áp dụng các kỹ thuật Object-Oriented Programming (OOP) hiện đại vào hệ thống nhúng (ESP32/Arduino Framework).

## 1. Tổng quan Kiến trúc (Clean Architecture)

Dự án loại bỏ hoàn toàn cách viết code kiểu "spaghetti" (gom toàn bộ logic vào `setup()` và `loop()` trong file `.ino`). Thay vào đó, hệ thống được chia thành nhiều lớp (layers) và module độc lập:

- **Core Layer:** Chứa lõi điều phối (`Application`) và các Interface (`ILogger`, `IWiFiManager`, `IDisplay`,...). Lớp này không phụ thuộc vào bất kỳ thư viện phần cứng cụ thể nào (ngoại trừ các kiểu dữ liệu chuẩn).
- **Service Layer:** (`VoiceService`) Chứa Business Logic chính. Ở đây là quy trình: "Ghi âm -> Gửi lên HTTP Server -> Nhận phản hồi". Service layer điều phối các interface của Network và Audio.
- **Hardware/Infrastructure Layer:** (`WiFiManager`, `DisplayManager`, `ButtonManager`, `AudioRecorder`, `HttpClient`). Đây là nơi duy nhất import và tương tác trực tiếp với các thư viện phần cứng của ESP32 (`WiFi.h`, `Adafruit_SSD1306.h`, `driver/i2s.h`, `HTTPClient.h`).

## 2. Dependency Injection (DI)

Không một class nào trong hệ thống tự khởi tạo (dùng toán tử `new` hoặc gọi hàm khởi tạo) các dependencies của nó.
Mọi dependency đều được truyền vào (inject) thông qua constructor.

```cpp
// Ví dụ Dependency Injection trong main.ino
Logger logger;
AudioRecorder audioRecorder(logger);
WiFiManager wifiManager(logger);
HttpClient httpClient(logger, wifiManager);

VoiceService voiceService(logger, audioRecorder, httpClient);
```

Việc này giúp:
- **Low Coupling (Khớp nối lỏng):** Các module không bị dính chặt vào nhau.
- **Testability:** Có thể dễ dàng thay thế `AudioRecorder` thật bằng một `MockAudioRecorder` trong lúc Unit Test.

## 3. SOLID Principles

- **Single Responsibility Principle (SRP):** 
  - `Logger`: Chỉ làm duy nhất một việc là in log ra Serial. Là module duy nhất gọi `Serial.print`.
  - `DisplayManager`: Chỉ chịu trách nhiệm điều khiển OLED.
  - `HttpClient`: Chỉ tạo request, gửi data và parse JSON.
- **Open/Closed Principle (OCP):**
  - Hệ thống dễ dàng mở rộng. Ví dụ muốn thêm tính năng gửi qua MQTT thay vì HTTP, ta chỉ cần tạo `MqttClient` implement một interface chung mà không phải sửa `VoiceService`.
- **Liskov Substitution Principle (LSP):**
  - `Application` sử dụng con trỏ/reference đến `IDisplay`. Ta có thể thay thế `DisplayManager` (dùng SSD1306) bằng `LcdDisplayManager` (dùng LCD 16x2) mà hệ thống vẫn chạy bình thường.
- **Interface Segregation Principle (ISP):**
  - Các interfaces được chia nhỏ và cụ thể: `IWiFiManager`, `IButton`, `IAudioRecorder`. Không có interface nào "ôm đồm" quá nhiều chức năng.
- **Dependency Inversion Principle (DIP):**
  - Các module bậc cao (`Application`, `VoiceService`) không phụ thuộc vào các module bậc thấp (`DisplayManager`, `WiFiManager`). Cả hai đều phụ thuộc vào Abstractions (Interfaces).

## 4. Quản lý Bộ nhớ (RAII)

ESP32 có hạn chế nghiêm ngặt về RAM (đặc biệt là dung lượng Stack ~8KB). Do cấu hình POC yêu cầu lưu trữ âm thanh PCM lên đến 96KB (16kHz, 16bit, 3 giây), việc sử dụng Stack là không thể.

Tuy nhiên, nguyên tắc thiết kế cấm sử dụng `malloc` và `free`. Do đó, tài nguyên vùng nhớ Heap được quản lý tự động qua cơ chế **RAII** (Resource Acquisition Is Initialization) bằng việc sử dụng `std::vector<uint8_t>`.
- Bộ nhớ tự động được giải phóng hoặc quản lý vòng đời khi `std::vector` bị hủy.
- Khắc phục nguy cơ Memory Leak thường thấy trong C.

## 5. Cấu trúc Thư mục

- `config/`: Cấu hình hệ thống (Pin, WiFi, Baudrate).
- `models/`: Các Data struct/class (VD: `VoiceResult` trả về từ HTTP).
- `core/`: Giao diện (Interfaces), Logger, và trình điều phối Application.
- `wifi/`, `network/`: Quản lý kết nối mạng và giao thức truyền tải.
- `display/`, `button/`, `audio/`: Tương tác ngoại vi phần cứng.
- `services/`: Business Logic.

## 6. Luồng Hoạt động (Workflow)

1. `main.ino` thiết lập toàn bộ sơ đồ DI.
2. `app.begin()` kích hoạt kết nối WiFi, OLED, và I2S.
3. `app.loop()` duy trì WiFi keep-alive và đọc trạng thái nút nhấn.
4. Khi nút nhấn được kích hoạt (GPIO 0 kéo xuống mức LOW), `app` gọi `voiceService.processVoice()`.
5. `VoiceService` ra lệnh `recorder` thu âm, trích xuất audio buffer, truyền vào `httpClient.sendAudio()`.
6. `HttpClient` gửi multipart/raw data, parse kết quả JSON trả về object `VoiceResult`.
7. `Application` nhận kết quả từ Service và hiển thị lên màn hình OLED.
