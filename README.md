# Robot AI ESP32 - Voice Assistant (Gemini API)

Dự án Robot AI sử dụng vi điều khiển ESP32-S3, tích hợp nhận diện giọng nói (Speech-To-Text) thông qua giao thức I2S, hiển thị OLED và giao tiếp với Python Server sử dụng Google Gemini AI để phản hồi thông minh.

## 🛠 Yêu cầu phần cứng
- **Vi điều khiển:** ESP32-S3 (Bản N16R8 hoặc tương đương).
- **Microphone:** INMP441 (Giao thức I2S 24-bit/16-bit).
- **Màn hình:** OLED 0.96 inch (Giao thức I2C).
- **Nút bấm:** Sử dụng nút BOOT có sẵn trên mạch ESP32 (GPIO 0).

## 🔌 Sơ đồ nối dây (Pinout)

### INMP441 (Microphone)
- **VCC:** 3.3V
- **GND:** GND
- **L/R:** GND (Bắt buộc nối GND để thiết lập kênh Left)
- **SCK:** GPIO 16 (I2S Clock)
- **WS:** GPIO 17 (Word Select / L/R Clock)
- **SD:** GPIO 18 (Serial Data IN)

### OLED 0.96 inch (I2C)
- **VCC:** 3.3V
- **GND:** GND
- **SCL:** GPIO 12
- **SDA:** GPIO 11

### Nút Bấm (Thu âm)
- Tích hợp sẵn trên nút **BOOT** của ESP32-S3 (GPIO 0).
- *Lưu ý: Driver I2S đã được thiết lập `mck_io_num = I2S_PIN_NO_CHANGE` để tránh xung đột MCLK với nút BOOT.*

## ⚙️ Cài đặt phần mềm (ESP32)

1. Mở file `config/Secrets.h` và cấu hình WiFi:
```cpp
#define WIFI_SSID "Tên_WiFi"
#define WIFI_PASSWORD "Mật_khẩu"
```
2. Trong `src/config/AppConfig.h`, cấu hình IP của Server Python:
```cpp
const char* const VOICE_API_URL = "http://<IP_SERVER_PYTHON>:5000/upload";
```
3. Cài đặt các thư viện Arduino cần thiết:
   - `Adafruit_SSD1306` (cho OLED)
   - `ArduinoJson` (để parse JSON phản hồi)
4. Biên dịch và nạp code lên ESP32-S3.

## 🧠 Cài đặt Python Server & Gemini API

Server Python đóng vai trò nhận file âm thanh chuẩn (PCM/WAV) từ ESP32, xử lý nhận diện giọng nói thông qua Google STT, sau đó đưa văn bản vào Gemini AI để lấy câu trả lời thông minh.

1. Chuyển đến thư mục chứa server (ví dụ `D:\arduino\python`).
2. Cài đặt các thư viện Python:
```bash
pip install flask SpeechRecognition wave google-generativeai
```
3. Mở file `server.py` và thêm API Key Gemini của bạn:
```python
GEMINI_API_KEY = "YOUR_GEMINI_API_KEY"
```
4. Khởi chạy Server:
```bash
python server.py
```

## 🚀 Hướng dẫn sử dụng
1. Cấp nguồn cho ESP32, màn hình OLED sẽ hiển thị trạng thái `Connecting...` và sau đó là `Ready`.
2. **Nhấn và giữ** nút BOOT trên ESP32 để bắt đầu thu âm. Màn hình hiện `Recording[*]`.
3. Đưa sát miệng vào Micro INMP441 và nói rõ ràng (ví dụ: *"Hôm nay là thứ mấy?"*).
4. Thả nút BOOT ra, ESP32 sẽ nén âm thanh và gửi lên Server Python qua HTTP POST. Màn hình hiện `Processing...`.
5. Sau 2-3 giây, câu trả lời thông minh từ Gemini AI sẽ được trả về và in lên màn hình OLED.

## 💡 Kiến trúc thuật toán I2S
Dự án sử dụng cấu hình I2S tối ưu nhất (chuẩn DroneBotWorkshop) cho INMP441 trên ESP32:
- Tần số lấy mẫu: `16000 Hz` (chuẩn nhận diện giọng nói).
- Định dạng: `16-bit`, kênh `ONLY_LEFT`.
- Tránh xung đột phần cứng DMA và đảm bảo tín hiệu Audio không bị nhiễu tĩnh điện (Clipping) thông qua việc ánh xạ trực tiếp MSB.

