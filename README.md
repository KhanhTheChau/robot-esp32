<div align="center">
  <h1>🤖 Project MICA: Smart Healthcare Companion</h1>
  <p><strong>Bản lề cho thế hệ Robot Nhúng tự trị (Autonomous Embedded Robotics) giao tiếp bằng Giọng nói</strong></p>

  <p>
    <img src="https://img.shields.io/badge/Hardware-ESP32--S3-orange.svg" alt="Hardware" />
    <img src="https://img.shields.io/badge/Language-C++%20%7C%20Python-blue.svg" alt="Languages" />
    <img src="https://img.shields.io/badge/Architecture-Edge%20Computing-success.svg" alt="Architecture" />
    <img src="https://img.shields.io/badge/AI_Engine-Gemini%201.5-yellow.svg" alt="AI Engine" />
  </p>
</div>

---

## 🌟 Tầm nhìn Dự án (Vision)

Đây không chỉ là một món đồ chơi điện tử lắp ráp thông thường. **MICA Robot** là một giải pháp nguyên mẫu cấp độ Doanh nghiệp (Enterprise-grade Prototype) nhằm chứng minh khả năng đưa AI tạo sinh (Generative AI) vào thiết bị chăm sóc sức khỏe thông minh (Healthcare Edge Devices) với **Độ trễ gần như bằng không (Ultra-low Latency)**.

Thay vì bắt người dùng phải bấm nút để nói chuyện như bộ đàm, hệ thống sử dụng thuật toán **VAD (Voice Activity Detection)** phân tích sóng âm hoàn toàn tại bộ xử lý nhúng (Edge Computing), biến Robot thành một trợ lý luôn lắng nghe thụ động và phản hồi tức thì. 

Dự án này là nền móng kiến trúc chuẩn mực để bạn có thể mở rộng lên các hệ thống lớn hơn (như Smart Home Control, Receptionist Robot, hoặc AI Companion).

## 🚀 Tính năng Cốt lõi & Đột phá Công nghệ

* 🎙️ **Edge VAD & Hands-free:** 100% rảnh tay. Cảm biến VAD chạy trực tiếp trên lõi ESP32, phân tích RMS liên tục và chỉ mở luồng mạng khi có tiếng người. Không lãng phí băng thông mạng, không tốn API.
* ⚡ **WebSocket Continuous Streaming:** Giao tiếp Song công toàn phần (Full-Duplex). Âm thanh được nén thành luồng dữ liệu nhị phân (Binary PCM) và đẩy đi liên tục, loại bỏ hoàn toàn độ trễ khổng lồ của HTTP/RESTful thông thường.
* 🧠 **Hệ sinh thái AI Tối thượng:** 
  * **Tai:** `Google Speech Recognition` (STT).
  * **Não:** `Google Gemini 1.5` phân tích ngữ cảnh siêu việt.
  * **Miệng:** `Microsoft Edge-TTS` (Neural Voice) cho giọng điệu trẻ em cực kỳ biểu cảm, tự nhiên.
* 🎭 **UI Cảm xúc Thời gian thực:** LLM phân tích cảm xúc ẩn trong câu trả lời (Vui, Buồn, Giận, Ngạc nhiên) và kích hoạt trực tiếp biểu cảm đồ họa động trên màn hình OLED bằng kỹ thuật vẽ vector siêu nhẹ.
* 🧩 **Kiến trúc Mã nguồn Sạch (Clean Architecture):** Mã nguồn ESP32 được xây dựng trên nguyên tắc **OOP**, **Dependency Injection** và **State Machine**, loại bỏ hoàn toàn mã "Spaghetti". Khả năng bảo trì và thêm mới phần cứng (như tay máy, camera, cảm biến) là vô tận.

---

## 📚 Bách khoa toàn thư Tài liệu (Documentation)

Hệ thống tài liệu được thiết kế không gắn số cứng, giúp dễ dàng mở rộng và bổ sung thêm các nhánh (module) mới trong tương lai khi dự án Scale-up.

> **Khuyến nghị:** Hãy đọc tài liệu theo trình tự dưới đây nếu bạn là người mới bắt đầu.

### HỆ THỐNG & KIẾN TRÚC
* 🗺️ [**System Architecture:**](docs/System_Architecture.md) Bức tranh tổng thể về Giao thức mạng, Kiến trúc Client-Server và Chu trình sống của dữ liệu.
* 💻 [**Firmware Architecture:**](docs/Firmware_Architecture.md) Giải phẫu mã nguồn C++ trên ESP32, Dependency Injection và State Machine.

### PHẦN CỨNG & GIAO TIẾP VẬT LÝ
* 🛠️ [**Hardware Setup:**](docs/Hardware_Setup.md) Sơ đồ Pinout I2S, I2C, hướng dẫn xử lý sụt nguồn, triệt tiêu nhiễu sóng WiFi và Ground Loop.
* 🔊 [**Audio & VAD:**](docs/Audio_and_VAD.md) Thuật toán Căn bậc hai trung bình (RMS), cách Dò sóng (Tuning) và hướng xử lý Tiếng vọng (Acoustic Echo).
* 📺 [**OLED Display & UI:**](docs/OLED_Display_UI.md) Quản lý bộ nhớ `PROGMEM`, thủ thuật Non-blocking 20fps Animation và thuật toán vẽ khuôn mặt.

### XỬ LÝ ĐÁM MÂY (CLOUD & BACKEND)
* ☁️ [**Server Backend:**](docs/Server_Backend.md) Cơ chế vận hành của Python Asyncio, WebSocket Server, FFmpeg Audio Chunking và API Tích hợp AI.

---

## ⚙️ Hướng dẫn Cài đặt Môi trường (Quick Start)

### 1. Phía Server (Máy tính / Raspberry Pi)
Chạy Terminal tại thư mục `python/`:
```bash
# Cài đặt thư viện AI và luồng dữ liệu
pip install websockets asyncio SpeechRecognition wave google-generativeai edge-tts imageio-ffmpeg
```
Mở file `server.py`, điền key của bạn:
```python
genai.configure(api_key="API_KEY_GEMINI_CUA_BAN")
```
Khởi động cụm Server:
```bash
python server.py
```

### 2. Phía Nhúng (ESP32-S3)
> 🆕 **Dành cho Người mới bắt đầu (Beginner):** Nếu bạn chưa từng dùng C++ hay IDE của Arduino bao giờ, hãy đọc qua cuốn sổ tay [Cầm tay chỉ việc Nạp code từ A-Z](docs/Arduino_Beginner_Guide.md) này trước nhé!

1. Cài đặt các thư viện trong Arduino IDE: `ArduinoJson`, `Adafruit SSD1306`, `Adafruit GFX`, `WebSockets` (by Markus Sattler).
2. Vào `src/config/Secrets.example.h`, đổi tên thành `Secrets.h` và điền SSID/Mật khẩu WiFi.
3. Vào `src/config/AppConfig.h`, cấu hình lại `WS_API_IP` trỏ đến đúng IP máy chủ Python của bạn.
4. Nạp code xuống board ESP32-S3 và tận hưởng thành quả!

---

## 🤝 Đóng góp & Mở rộng tương lai (Roadmap)
Dự án được thiết kế với tư duy **Scale-up Ready**. Các Module tương lai có thể dễ dàng được nhúng vào như:
- Tích hợp Motor Servo điều khiển đầu Robot lắc lư theo cảm xúc.
- Đọc hình ảnh (Vision) phân tích người đối diện bằng ESP32-CAM.
- Local LLM chạy trên Raspberry Pi thay thế cho Cloud API.
