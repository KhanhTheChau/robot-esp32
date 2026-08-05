# 🧠 Myka AI Server Backend

Chào mừng đến với Bộ não Đám mây (Cloud Brain) của Robot Myka. Đây là một máy chủ bất đồng bộ (Asynchronous Server) được viết hoàn toàn bằng Python, có nhiệm vụ giao tiếp thời gian thực (Real-time) với vi điều khiển ESP32 thông qua giao thức WebSocket.

Máy chủ này đóng vai trò là "Nhạc trưởng" điều phối toàn bộ hệ sinh thái AI, biến những tín hiệu âm thanh thô sơ (Raw PCM) từ ESP32 trở thành một chuỗi hội thoại có cảm xúc.

---

## ⚡ Các Tính năng Nổi bật

- **Full-Duplex WebSocket:** Truyền tải dữ liệu âm thanh nhị phân (Binary Streaming) hai chiều không độ trễ, loại bỏ hoàn toàn Overhead của HTTP.
- **Microservice AI Đa tầng:**
  - **Nhận diện giọng nói (STT):** Sử dụng `SpeechRecognition` để dịch nguyên âm thanh (PCM 16kHz Mono) thành văn bản.
  - **Tư duy Ngôn ngữ (LLM):** Sử dụng `Google Gemini 1.5 Flash` qua SDK chính thức để xử lý văn bản, tạo ra câu trả lời và tự động gán nhãn Cảm xúc (Emotion).
  - **Tổng hợp Giọng nói (TTS):** Sử dụng `Microsoft Edge-TTS` để chuyển câu trả lời thành giọng nói trẻ em sinh động.
- **Xử lý Đồng thời (Concurrency) & Ép xung thời gian (Throttling):** Máy chủ có khả năng xử lý TTS và đẩy âm thanh xuống ESP32 ở tốc độ chuẩn xác 32,000 bytes/s (16-bit, 16kHz). Việc chạy song song luồng âm thanh và luồng suy nghĩ giúp giấu kín hoàn toàn độ trễ của AI.
- **Xử lý Cục bộ (Local Intent Detection):** Nhận diện trực tiếp các lệnh cơ bản (như "tạm biệt", "đi ngủ") ở tầng STT bằng toán tử `in` để giảm thiểu việc gọi API không cần thiết.
- **Tự động Ghi nhật ký (Conversation Logging):** Lưu lại mọi hội thoại giữa Robot và Người dùng vào file `chat_history.log` để tiện tra cứu và kiểm duyệt.

---

## ⚙️ Hướng dẫn Cài đặt & Khởi chạy

### 1. Yêu cầu Hệ thống
- Hệ điều hành: Windows, macOS, hoặc Linux (khuyến nghị chạy trên Raspberry Pi hoặc Edge Server).
- Ngôn ngữ: **Python 3.10** trở lên.

### 2. Cài đặt Thư viện
Mở Terminal tại thư mục `server/` và chạy lệnh sau để tự động cài đặt tất cả thư viện cần thiết:
```bash
pip install -r requirements.txt
```

### 3. Cấu hình Bảo mật (API Keys)
Dự án áp dụng tiêu chuẩn bảo mật biến môi trường để tuyệt đối không rò rỉ API Key lên Github:
1. Sao chép file `.env.example` thành file `.env`.
2. Mở file `.env` vừa tạo và điền **Gemini API Key** của bạn vào:
```ini
GEMINI_API_KEY_1=AIzaSy... (API Key của bạn)
```
*(Lưu ý: File `.env` và thư mục `audio_cache/` đã được tự động đưa vào `.gitignore`)*.

### 4. Khởi động Máy chủ
```bash
python server.py
```
Nếu Terminal hiện dòng chữ `[SERVER] ESP32 đã kết nối WebSocket!`, tức là bộ não đã liên kết thành công với phần cứng và sẵn sàng hoạt động.

---

## 📁 Cấu trúc Thư mục

```text
server/
├── core/
│   ├── client_session.py   # Máy phát trạng thái (State Machine) của Server. Xử lý các luồng WAKE/SLEEP/SPEAKING.
│   ├── llm_engine.py       # Quản lý Gemini API và trích xuất Cảm xúc (Emotion).
│   ├── phrase_manager.py   # Hệ thống Cache MP3 cho các câu giao tiếp cơ bản ("Chờ cháu chút", "Tạm biệt").
│   ├── stt_engine.py       # Quản lý Google Speech-to-Text và nhận diện từ khóa đánh thức cục bộ.
│   └── tts_engine.py       # Tích hợp Edge-TTS tạo ra các tệp PCM thô.
├── audio_cache/            # Thư mục sinh tự động để lưu trữ các file âm thanh tạm thời.
├── .env.example            # Mẫu file cấu hình bảo mật.
├── requirements.txt        # Danh sách thư viện Python.
└── server.py               # Entry point - Khởi chạy WebSocket Server ở Port 5000.
```

---

## 🤝 Đóng góp và Mở rộng
Mô hình hiện tại (ClientSession) đã được tối ưu cho kiến trúc đa luồng (Multi-threading). Để Scale-up dự án, bạn hoàn toàn có thể thay thế class `LLMEngine` bằng một local model (như `llama.cpp`) hoặc đổi `STTEngine` sang Whisper API mà không làm thay đổi bất kỳ logic giao tiếp nào với Robot ESP32.
