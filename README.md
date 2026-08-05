<div align="center">
  <h1>🤖 Project Myka: Autonomous Smart Healthcare Companion</h1>
  <p><strong>Nền tảng Tiên phong cho thế hệ Robot Nhúng tự trị (Autonomous Embedded Robotics) giao tiếp bằng Giọng nói thời gian thực.</strong></p>

  <p>
    <img src="https://img.shields.io/badge/Hardware-ESP32--S3-orange.svg" alt="Hardware" />
    <img src="https://img.shields.io/badge/Firmware-C%2B%2B%20%28OOP%29-blue.svg" alt="Firmware" />
    <img src="https://img.shields.io/badge/Backend-Python%20Asyncio-blue.svg" alt="Backend" />
    <img src="https://img.shields.io/badge/AI_Engine-Gemini%201.5%20Flash-yellow.svg" alt="AI Engine" />
  </p>
</div>

---

## 🌟 Tầm nhìn Dự án (Vision & Scope)

**Myka Robot** là một giải pháp nguyên mẫu cấp độ Doanh nghiệp (Enterprise-grade Prototype) nhằm chứng minh tính khả thi của việc tích hợp AI tạo sinh (Generative AI) vào các thiết bị chăm sóc sức khỏe biên (Healthcare Edge Devices) với **Độ trễ siêu thấp (Ultra-low Latency)**.

Hệ thống loại bỏ hoàn toàn các nút bấm vật lý. Thông qua thuật toán **Voice Activity Detection (VAD)** chạy nội bộ trên vi xử lý nhúng, Robot hoạt động như một thực thể luôn lắng nghe thụ động (Always-on passive listening) nhưng vẫn đảm bảo tuyệt đối quyền riêng tư và tiết kiệm năng lượng. Kiến trúc phân tán (Edge-to-Cloud) này mở ra tiềm năng thương mại hóa cho các mảng Smart Home, Lễ tân tự động (Receptionist Robot), hoặc Trợ lý y tế ảo.

## 🚀 Tính năng Cốt lõi & Đột phá Công nghệ

* 🎙️ **Edge VAD & Chống dội âm (Acoustic Isolation):** Cảm biến năng lượng âm thanh (RMS VAD) chạy trực tiếp trên lõi ESP32. Kết hợp với kỹ thuật Bán song công (Half-Duplex), Robot loại bỏ triệt để hiện tượng tự nghe chính mình, đồng thời tiết kiệm 100% băng thông tĩnh.
* ⚡ **WebSocket Continuous Streaming:** Giao thức giao tiếp TCP nội bộ. Âm thanh được đóng gói thành luồng nhị phân (Binary PCM) truyền tải liên tục không ngắt quãng, vượt trội hoàn toàn so với mô hình HTTP/RESTful truyền thống về mặt tốc độ phản hồi.
* 🧠 **Hệ sinh thái AI Đa tầng:** 
  * **Xử lý Ngôn ngữ (NLP):** `Google Gemini 1.5 Flash` cung cấp khả năng phân tích ngữ cảnh siêu việt.
  * **Xử lý Giọng nói (STT/TTS):** Tích hợp chéo `Google Speech Recognition` (Đầu vào) và `Microsoft Edge-TTS` Neural Voice (Đầu ra).
* 🎭 **UI Cảm xúc & Chuyển động Mắt (RoboEyes):** Khung hình OLED 128x64 hiển thị biểu cảm động. Thuật toán xử lý nội bộ tự động tính toán tọa độ mắt để điều khiển khuôn miệng nhóp nhép đồng bộ hoàn hảo với luồng âm thanh phát ra (Lip-syncing animation).
* 🔒 **Bảo mật Cấp Doanh nghiệp:** Toàn bộ API Key, thông tin mạng WiFi đều được tách biệt vào các tệp tin cấu hình độc lập (`.env` và `Secrets.h`), không bao giờ bị lộ (commit) lên mã nguồn mở.
* 📝 **Ghi nhật ký Hội thoại (Conversation Logging):** Server tự động lưu trữ và quản lý lịch sử hội thoại thành các file log cục bộ phục vụ cho công tác bảo trì và kiểm duyệt.

---

## 📚 Bách khoa toàn thư Hệ thống (Documentation)

Hệ thống tài liệu được chuẩn hóa dưới dạng Báo cáo Kỹ thuật, thiết kế theo mô-đun để phục vụ quá trình R&D và Scale-up.

### HỆ THỐNG & KIẾN TRÚC
* 🗺️ [**System Architecture:**](docs/System_Architecture.md) Bức tranh tổng thể về Giao thức mạng, Kiến trúc Client-Server và Chu trình sống của dữ liệu.
* 💻 [**Firmware Architecture:**](docs/Firmware_Architecture.md) Giải phẫu mã nguồn C++ trên ESP32, ứng dụng Dependency Injection và State Machine.

### PHẦN CỨNG & GIAO TIẾP VẬT LÝ
* 🛠️ [**Hardware Setup:**](docs/Hardware_Setup.md) Sơ đồ Pinout I2S, I2C, hướng dẫn xử lý sụt nguồn, triệt tiêu nhiễu sóng WiFi và Ground Loop.
* 🔊 [**Audio & VAD:**](docs/Audio_and_VAD.md) Báo cáo phân tích thuật toán RMS VAD, hệ quy chiếu ngành và hướng xử lý tiếng vọng (Acoustic Echo).
* 📺 [**OLED Display & UI:**](docs/OLED_Display_UI.md) Quản lý bộ nhớ `PROGMEM`, thủ thuật Non-blocking 20fps Animation và thuật toán vẽ khuôn mặt động.

### XỬ LÝ ĐÁM MÂY (CLOUD BACKEND)
* ☁️ [**Server Backend:**](docs/Server_Backend.md) Cơ chế vận hành của Python Asyncio, WebSocket Server và hệ thống Multi-threading.

---

## ⚙️ Hướng dẫn Triển khai (Deployment Guide)

### 1. Phía Máy chủ Đám mây / Edge PC (Python Backend)
Môi trường yêu cầu: Python 3.10+
Chạy Terminal tại thư mục `server/`:
```bash
# Cài đặt toàn bộ môi trường và thư viện
pip install websockets asyncio SpeechRecognition google-generativeai edge-tts python-dotenv
```
Thiết lập Bảo mật:
Đổi tên file `server/.env.example` thành `server/.env` và điền Google Gemini API Key của bạn vào:
```ini
GEMINI_API_KEY_1=API_KEY_CUA_BAN
```
Khởi động cụm Server:
```bash
python server.py
```
*(Server sẽ tự động lắng nghe ở Port 5000 và sinh file log hội thoại tại `chat_history.log`)*

### 2. Phía Vi điều khiển Nhúng (ESP32-S3)
> 🆕 **Lưu ý:** Vui lòng sử dụng Framework chuẩn của Arduino IDE.

1. Cài đặt các thư viện phụ thuộc: `ArduinoJson`, `Adafruit SSD1306`, `Adafruit GFX`, `WebSockets` (by Markus Sattler).
2. Vào thư mục `src/config/`, đổi tên file `Secrets.example.h` thành `Secrets.h` và điền SSID/Mật khẩu WiFi nội bộ của bạn. *(File này đã được bảo mật khỏi Git)*.
3. Vào `src/config/AppConfig.h`, cấu hình lại hằng số `WS_API_IP` trỏ đến đúng địa chỉ IP LAN của máy chủ Python.
4. Biên dịch (Compile) và nạp (Flash) mã nguồn C++ xuống board mạch ESP32-S3.

---

## 🤝 Lộ trình Nâng cấp (Roadmap & Scaling)
Dự án được kiến trúc với tư duy **Scale-up Ready**. Các Module nâng cao có thể lập tức được "plug-and-play":
- **Servo Motor Control:** Tích hợp động cơ Servo điều khiển trục cổ Robot lắc lư theo phổ cảm xúc.
- **Computer Vision:** Bổ sung ESP32-CAM để nhận diện khuôn mặt người đối diện.
- **Local LLM:** Triển khai LLaMA hoặc Mistral trên Raspberry Pi 5 để hoàn toàn độc lập với Internet (Offline Mode).
