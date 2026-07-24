# ESP32 Audio Proof of Concept (POC)

Dự án này là một bản thử nghiệm (Proof of Concept) cho hệ thống nhúng trên nền tảng **ESP32** (Sử dụng Arduino Framework) có khả năng ghi âm giọng nói qua Microphone I2S và gửi lên server để nhận diện.

Điểm nổi bật nhất của dự án là việc áp dụng triệt để các nguyên lý thiết kế phần mềm hiện đại: **Clean Architecture**, **SOLID**, **Dependency Injection**, và **RAII**.

## Tính năng (POC)

- Khởi động và kết nối WiFi.
- Quản lý trạng thái và hiển thị thông tin lên màn hình OLED (SSD1306).
- Chờ người dùng nhấn nút BOOT (GPIO 0).
- Ghi âm 3 giây từ I2S Microphone (VD: INMP441).
- Gửi dữ liệu âm thanh (Raw PCM) lên HTTP Server.
- Nhận phản hồi dạng JSON từ Server, bóc tách (Parse) thông tin: `intent`, `text`, `confidence`.
- Hiển thị kết quả nhận diện lên OLED và in log ra Serial.

## Cấu trúc thư mục

```text
├── config/       # Cấu hình phần cứng (Pin, I2S, WiFi credentials)
├── models/       # Data struct (VoiceResult)
├── core/         # Core Interfaces, Application logic, Logger
├── wifi/         # Xử lý kết nối WiFi
├── display/      # Xử lý màn hình OLED
├── button/       # Quản lý nút nhấn (Debounce)
├── audio/        # Xử lý I2S Microphone
├── network/      # HTTP Client và JSON Parsing
├── services/     # Business Logic (Voice Service)
├── docs/         # Tài liệu mô tả kiến trúc
└── main.ino      # Entry point, Dependency Injection Container
```

Chi tiết về thiết kế phần mềm, xin vui lòng xem tại: [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Phần cứng yêu cầu

1. **Board mạch**: ESP32 Development Board (ESP32-WROOM-32, ESP32-S3, v.v.)
2. **Microphone**: Module I2S Microphone (Ví dụ: INMP441)
3. **Màn hình**: OLED SSD1306 (Giao tiếp I2C)
4. **Nút bấm**: Tận dụng nút BOOT có sẵn trên board (GPIO 0).

### Sơ đồ đấu nối cơ bản

* **I2S Microphone (INMP441)**
  - SD (Data) -> GPIO 32
  - WS (LRCLK) -> GPIO 25
  - SCK (BCLK) -> GPIO 33
  - L/R -> GND (Kênh trái)
  - VDD -> 3.3V
  - GND -> GND
* **OLED SSD1306**
  - SDA -> Tùy board (Thường là GPIO 21)
  - SCL -> Tùy board (Thường là GPIO 22)
  - VCC -> 3.3V
  - GND -> GND

*(Các chân GPIO có thể thay đổi tùy ý trong file [config/AppConfig.h](config/AppConfig.h))*

## Hướng dẫn cài đặt và chạy thử (How to run)

### Bước 1: Cài đặt công cụ

Dự án tương thích với **Arduino IDE 2.x** hoặc **PlatformIO**.

Bạn cần cài đặt các thư viện sau thông qua Library Manager:
- `Adafruit GFX Library` (bởi Adafruit)
- `Adafruit SSD1306` (bởi Adafruit)
- `ArduinoJson` (bởi Benoit Blanchon)

### Bước 2: Điền thông tin cấu hình

Mở file `config/Secrets.h` và thay đổi thông tin WiFi của bạn:
```cpp
namespace Secrets
{
    constexpr const char* WIFI_SSID = "Tên_WiFi_Của_Bạn";
    constexpr const char* WIFI_PASSWORD = "Mật_Khẩu_WiFi";
}
```

Mở file `config/AppConfig.h` và cấu hình lại URL của HTTP Server nhận file âm thanh:
```cpp
namespace AppConfig
{
    // ...
    constexpr const char* VOICE_API_URL = "http://địa_chỉ_ip_server_của_bạn:port/api/voice";
}
```

### Bước 3: Biên dịch và Nạp code

1. Mở file `main.ino` trong Arduino IDE.
2. Chọn Board là ESP32 Dev Module (hoặc board tương ứng của bạn).
3. Cấu hình Partition Scheme: Chọn tối thiểu **"Huge APP (3MB No OTA/1MB SPIFFS)"** vì thư viện WiFi và mã giả lập chiếm khá nhiều bộ nhớ Flash.
4. Bấm **Upload** (Nạp).

### Bước 4: Test luồng hoạt động

1. Mở Serial Monitor (Baudrate: `115200`).
2. Xem log hệ thống khởi động. Màn hình OLED sẽ hiển thị trạng thái đang kết nối WiFi và "Ready" khi sẵn sàng.
3. Nhấn và thả nút **BOOT** trên mạch ESP32.
4. Màn hình báo "Recording..." (Bắt đầu nói vào Microphone trong 3 giây).
5. Hệ thống tiến hành gửi data lên server, nhận JSON và in kết quả ra màn hình.
