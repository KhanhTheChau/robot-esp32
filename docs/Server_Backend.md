# Phân tích Python Backend Server (WebSocket)

Nằm tại thư mục `D:\arduino\server\`, kịch bản Python đóng vai trò là "Bộ não" trung tâm của Robot. Việc đưa phần xử lý nặng nề lên Server mang lại lợi ích tuyệt đối cho hệ thống Nhúng. Server được viết theo cấu trúc OOP (Hướng đối tượng) giúp dễ mở rộng.

## 0. Cấu trúc Source Code

- **`server.py`**: Entry point khởi động Server WebSocket.
- **`core/client_session.py`**: Quản lý State Machine (`SLEEP`, `AWAKE`) cho từng kết nối.
- **`core/phrase_manager.py`**: Bộ quản lý các câu thoại tĩnh (Wake up, Thinking, Goodbye). Pre-generate thành file PCM lúc khởi động để stream tức thì mà không cần sinh TTS.
- **`core/stt_engine.py`**: Google STT và kiểm tra Wake Word ngẫu nhiên.
- **`core/llm_engine.py`**: Bọc Gemini API để lấy câu trả lời và cảm xúc.
- **`core/tts_engine.py`**: Quản lý Edge TTS và convert qua FFmpeg.

## 1. Giao thức WebSocket Đa luồng (Asyncio)

Server sử dụng thư viện `websockets` và `asyncio` để tạo ra một máy chủ WebSocket Non-blocking (Hoạt động bất đồng bộ).

- **Port hoạt động:** `5000` (Binding vào `0.0.0.0` để các thiết bị trong mạng LAN có thể kết nối).
- **Khả năng:** Chấp nhận kết nối từ ESP32, cho phép truyền và nhận cả 2 định dạng:
  - **Dữ liệu nhị phân (Binary Frames):** Dùng để stream RAW PCM Audio.
  - **Dữ liệu chữ (Text Frames):** Dùng để trao đổi các lệnh JSON điều khiển.

## 2. Quy trình Xử lý (Pipeline)

Quy trình diễn ra khi Server nhận được đoạn âm thanh thu âm từ ESP32:

1. **Nhận và Lưu trữ PCM (Receive & Accumulate):**
   - Server nhận các gói tin nhị phân và nối (append) chúng vào mảng byte `audio_buffer`.
2. **Nhận lệnh Kết thúc (End of Speech):**
   - Khi nhận được JSON `{"action": "end_of_speech"}`, Server tiến hành đóng băng `audio_buffer` và chuyển sang `process_audio()`.
3. **Phát hiện Wake Word & Phản hồi nhanh:**
   - Khi Robot đang `SLEEP`, Server chạy STT kiểm tra xem có tên (myka, mi ca...) không. Nếu có, chuyển sang `AWAKE`, phát ngẫu nhiên một câu WAKE_UP từ `phrase_manager` (đã cache sẵn).
   - Khi Robot đang `AWAKE`, lập tức phát ngẫu nhiên một câu THINKING để báo cho người dùng biết AI đang xử lý.
4. **Speech-to-Text (STT):**
   - Chuyển mảng RAW PCM thành đối tượng AudioData.
   - Gửi file này lên **Google Speech Recognition** (Miễn phí, nhận diện Tiếng Việt khá tốt).
5. **LLM & Emotion Extraction (Gemini 1.5/3.1):**
   - Sử dụng thư viện `google.generativeai` để ném câu hỏi của bạn lên cho AI.
   - Trích xuất định dạng JSON (Bắt buộc trả về `{ "text": "câu trả lời", "emotion": "cảm xúc" }`) để ESP32 có thể phân tích dễ dàng. Nếu intent là tạm biệt, gán emotion là "goodbye".
6. **Text-to-Speech (Edge-TTS):**
   - Dùng thư viện `edge-tts` (Giọng `vi-VN-HoaiMyNeural` - Tăng pitch và rate để tạo giọng trẻ em).
   - Tạo ra file MP3. Nếu là câu phản hồi từ LLM, sẽ sinh TTS tức thì.
7. **Băm Âm Thanh (FFmpeg Streaming):**
   - ESP32 không thể giải mã MP3. Server gọi `FFmpeg` (thông qua `subprocess`) để tách file MP3 thành một file RAW PCM 16-bit 16kHz (`s16le`).
   - Server đọc file PCM này, chẻ nhỏ thành từng khối `1024 bytes` và gửi xuống ESP32 liên tục qua WebSocket `send(chunk)`.

## 3. Các điểm Cần chú ý (Gotchas)

- **Lỗi Firewall Windows:** Nếu ESP32 báo "Connecting Server..." mãi không vào được, 99% là do Firewall của Windows đã chặn Port `5000` đối với Local Network. Hãy vào Windows Firewall thêm rule `Inbound` cho port `5000` hoặc chỉnh mạng về chế độ `Private`.
- **Lỗi FFmpeg:** Đảm bảo thư viện `imageio-ffmpeg` hoạt động, vì không có nó, Server không thể tạo ra file PCM và ESP32 sẽ chỉ nhận được im lặng.
- **Tài khoản Google Gemini:** Free-tier của Gemini bị giới hạn số request mỗi phút. Nếu bạn hỏi quá nhanh, bạn có thể nhận lỗi `ResourceExhausted`.
