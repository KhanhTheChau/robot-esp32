# Phân tích Python Backend Server (WebSocket)

Nằm tại file `server.py`, kịch bản Python đóng vai trò là "Bộ não" trung tâm của Robot. Việc đưa phần xử lý nặng nề lên Server mang lại lợi ích tuyệt đối cho hệ thống Nhúng.

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
   - Khi nhận được JSON `{"action": "end_of_speech"}`, Server tiến hành đóng băng `audio_buffer`.
3. **Speech-to-Text (STT):**
   - Chuyển mảng RAW PCM thành định dạng `WAV` trong bộ nhớ RAM bằng thư viện `wave`.
   - Gửi file `WAV` này lên **Google Speech Recognition** (Miễn phí, nhận diện Tiếng Việt khá tốt).
4. **LLM & Emotion Extraction (Gemini 1.5):**
   - Sử dụng thư viện `google.generativeai` để ném câu hỏi của bạn lên cho AI.
   - Trích xuất định dạng JSON (Bắt buộc trả về `{ "text": "câu trả lời", "emotion": "cảm xúc" }`) để ESP32 có thể phân tích dễ dàng.
5. **Text-to-Speech (Edge-TTS):**
   - Dùng thư viện `edge-tts` (Giọng `vi-VN-HoaiMyNeural` - Tăng pitch và rate để tạo giọng trẻ em).
   - Tạo ra file MP3 lưu tại thư mục `audio/`.
6. **Băm Âm Thanh (FFmpeg Streaming):**
   - ESP32 không thể giải mã MP3. Server gọi `FFmpeg` (thông qua `subprocess`) để tách file MP3 vừa tạo thành một file RAW PCM 16-bit 16kHz (`s16le`).
   - Server đọc file PCM này, chẻ nhỏ thành từng khối `1024 bytes` và gửi xuống ESP32 liên tục qua WebSocket `send(chunk)`.
   - Phát đồng thời ra loa máy tính (bằng `winsound` trên Windows) để làm demo.

## 3. Các điểm Cần chú ý (Gotchas)

- **Lỗi Firewall Windows:** Nếu ESP32 báo "Connecting Server..." mãi không vào được, 99% là do Firewall của Windows đã chặn Port `5000` đối với Local Network. Hãy vào Windows Firewall thêm rule `Inbound` cho port `5000` hoặc chỉnh mạng về chế độ `Private`.
- **Lỗi FFmpeg:** Đảm bảo thư viện `imageio-ffmpeg` hoạt động, vì không có nó, Server không thể tạo ra file PCM và ESP32 sẽ chỉ nhận được im lặng.
- **Tài khoản Google Gemini:** Free-tier của Gemini bị giới hạn số request mỗi phút. Nếu bạn hỏi quá nhanh, bạn có thể nhận lỗi `ResourceExhausted`.
