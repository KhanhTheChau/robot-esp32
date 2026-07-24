# ESP32-S3 Audio Proof of Concept (POC)

Dự án này là một bản thử nghiệm (Proof of Concept) cho hệ thống nhúng trên nền tảng **ESP32-S3** (Sử dụng Arduino Framework) có khả năng ghi âm giọng nói qua Microphone I2S và gửi lên server Python local để xử lý và nhận diện.

Điểm nổi bật nhất của dự án là việc áp dụng triệt để các nguyên lý thiết kế phần mềm hiện đại: **Clean Architecture**, **SOLID**, **Dependency Injection**, và **RAII**.

## Tính năng (POC)

- Khởi động và kết nối WiFi.
- Quản lý trạng thái và hiển thị thông tin lên màn hình OLED 0.96 inch (SSD1306, màu xanh).
- Chờ người dùng nhấn nút BOOT (GPIO 0).
- Ghi âm 3 giây từ I2S Microphone đa hướng (INMP441 MEMS).
- Gửi dữ liệu âm thanh (Raw PCM) lên HTTP Server cục bộ (chạy bằng Python).
- Nhận phản hồi dạng JSON từ Server, bóc tách (Parse) thông tin: `intent`, `text`, `confidence`.
- Hiển thị kết quả nhận diện lên OLED và in log ra Serial.

## Phần cứng được cấu hình sẵn

Dự án đã được cấu hình mặc định cho các linh kiện sau:
1. **Kit ESP32 S3 N16R8** (ESP32-S3 với 16MB Flash, 8MB PSRAM)
2. **Module Micro đa hướng I2S INMP441 MEMS**
3. **Màn hình OLED 0.96 inch (màu xanh)** (Giao tiếp I2C)

### Sơ đồ đấu nối trên ESP32-S3

* **I2S Microphone (INMP441)**
  - SCK (BCLK) -> **GPIO 16**
  - WS (LRCLK) -> **GPIO 17**
  - SD (Data/DOUT) -> **GPIO 18**
  - L/R -> GND (Kênh trái)
  - VDD -> 3.3V
  - GND -> GND
* **Màn hình OLED SSD1306**
  - SDA -> **GPIO 8**
  - SCL -> **GPIO 9**
  - VCC -> 3.3V
  - GND -> GND
* **Nút bấm**: Nút BOOT tích hợp sẵn (**GPIO 0**).

*(Nếu thay đổi chân, hãy cập nhật tại file [config/AppConfig.h](config/AppConfig.h))*

## Hướng dẫn cài đặt và chạy thử (How to run)

### Bước 1: Khởi động Python Server cục bộ

Mạch ESP32 sẽ gửi âm thanh lên server Python của bạn.
1. Đảm bảo máy tính của bạn và mạch ESP32 **kết nối cùng một mạng WiFi**.
2. Tìm **Địa chỉ IP LAN** của máy tính (Ví dụ: `192.168.1.5`). Bạn có thể mở CMD và gõ `ipconfig` để lấy địa chỉ IPv4.
3. Mở Terminal/CMD, di chuyển đến thư mục chứa server Python và chạy server:
   ```bash
   cd D:\arduino\python
   python server.py
   ```
*(Server mặc định thường lắng nghe trên cổng 5000, hãy đảm bảo tường lửa không chặn port này).*

### Bước 2: Cài đặt thư viện cho ESP32 trong Arduino IDE

Bạn cần cài đặt các thư viện sau thông qua Library Manager (Ctrl + Shift + I):
- `Adafruit GFX Library` (bởi Adafruit)
- `Adafruit SSD1306` (bởi Adafruit)
- `ArduinoJson` (bởi Benoit Blanchon)

### Bước 3: Điền thông tin cấu hình vào Code

1. Mở file `config/Secrets.h` và thay đổi thông tin WiFi:
   ```cpp
   namespace Secrets
   {
       constexpr const char* WIFI_SSID = "Tên_WiFi_Của_Bạn";
       constexpr const char* WIFI_PASSWORD = "Mật_Khẩu_WiFi";
   }
   ```
2. Mở file `config/AppConfig.h` và cập nhật IP tĩnh của máy tính vào API URL:
   ```cpp
   namespace AppConfig
   {
       // Thay 192.168.1.5 bằng IPv4 của máy tính bạn
       constexpr const char* VOICE_API_URL = "http://192.168.1.5:5000/upload";
       // ...
   }
   ```

### Bước 4: Cấu hình Board và Nạp Code

1. Mở file `main.ino` trong Arduino IDE.
2. Chọn Board là **ESP32S3 Dev Module**.
3. Bật PSRAM trong Arduino IDE vì kit S3 N16R8 có 8MB PSRAM:
   - `Tools` -> `PSRAM` -> **OPI PSRAM**
4. Đặt cấu hình Partition Scheme:
   - `Tools` -> `Partition Scheme` -> **16M Flash (3MB APP/9.9MB FATFS)** (Hoặc bất kỳ tùy chọn nào dành cho 16MB Flash).
5. Cắm cáp, chọn đúng cổng COM và bấm **Upload** (Nạp).

### Bước 5: Test luồng hoạt động

1. Mở Serial Monitor (Baudrate: `115200`).
2. Màn hình OLED (màu xanh) sẽ hiển thị trạng thái kết nối WiFi, báo "Ready" khi sẵn sàng.
3. Nhấn và thả nút **BOOT** trên mạch ESP32.
4. Màn hình báo "Recording..." (Bắt đầu nói vào Microphone INMP441 trong 3 giây).
5. Hệ thống gửi âm thanh qua LAN tới máy tính.
6. Máy tính (Python Server) xử lý âm thanh, trả kết quả. ESP32 nhận JSON và in kết quả ra màn hình OLED.

