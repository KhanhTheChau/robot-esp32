# Đề xuất Kiến trúc: Continuous Streaming với WebSocket & VAD

Tài liệu này trình bày giải pháp thay thế cơ chế "Bấm nút - Gửi HTTP" hiện tại bằng cơ chế **"Luôn lắng nghe - Truyền liên tục"** (Rảnh tay / Hands-free) sử dụng giao thức WebSocket và thuật toán Voice Activity Detection (VAD).

## 1. VAD (Voice Activity Detection) là gì? Có khả thi trên ESP32 không?

**VAD** là thuật toán giúp phân biệt đâu là tiếng người nói (Voice) và đâu là khoảng lặng hoặc tiếng ồn nền (Noise). 
**Minh chứng thực tế:** Hoàn toàn khả thi trên ESP32. Thực tế có 3 hướng tiếp cận VAD trên ESP32 đã được cộng đồng chứng minh:

1. **Energy-Based VAD (Dựa trên năng lượng/Âm lượng):** Tính toán Root Mean Square (RMS) của luồng âm thanh I2S. Nếu RMS vượt qua một ngưỡng (Threshold) trong một khoảng thời gian, ESP32 xác định là người đang nói. Phương pháp này siêu nhẹ, code thuần C++, lý tưởng cho Arduino IDE. *(Tham khảo: Các dự án máy đo độ ồn dB bằng INMP441).*
2. **Thư viện ESP32-SpeexDSP:** Một thư viện xử lý tín hiệu số mã nguồn mở (có sẵn trên Arduino/PlatformIO) cung cấp các hàm VAD, Lọc nhiễu (Noise Suppression) và quan trọng nhất là Khử tiếng vang (AEC - Acoustic Echo Cancellation). AEC rất cần thiết để VAD không bị kích hoạt ngược bởi chính tiếng loa của Robot.
3. **ESP-SR (VADNet):** Của chính Espressif cung cấp mạng nơ-ron VAD, tuy nhiên yêu cầu phải dùng ESP-IDF thay vì Arduino IDE.

**👉 Giải pháp đề xuất cho project này:** Bắt đầu bằng **Energy-Based VAD kết hợp bộ đệm thời gian (Hold-time)**. Nếu âm lượng đủ lớn, bắt đầu stream. Nếu im lặng quá 1.5 giây, ngắt stream.

## 2. Kiến trúc Hệ thống mới (WebSockets)

Thay vì dùng HTTP POST (phải đợi thu âm xong mới gửi toàn bộ file, gây độ trễ lớn), chúng ta sẽ dùng **WebSocket** để truyền dữ liệu thời gian thực (Real-time Streaming).

### Quy trình hoạt động (Mô hình rảnh tay)

1. **Kết nối:** ESP32 khởi động và mở một kết nối WebSocket (Persistent Connection) tới `ws://192.168.1.x:5000/stream` của Python Server.
2. **Lắng nghe (IDLE):** ESP32 liên tục đọc mic INMP441, tính toán RMS nhưng **chưa gửi** lên mạng. Màn hình vẫn phát ảnh GIF.
3. **Phát hiện giọng nói (VAD Trigger):** 
   - Khi bạn cất tiếng nói, RMS tăng vọt vượt ngưỡng. ESP32 lập tức bơm các gói PCM thô (Binary Websocket Frames - cỡ 1024 bytes/gói) liên tục lên Server.
   - Màn hình chuyển sang trạng thái "Listening...".
4. **Kết thúc câu nói:**
   - Khi bạn ngừng nói, RMS giảm xuống. ESP32 đếm ngược 1.5 giây (Khoảng lặng).
   - Nếu vẫn im lặng, ESP32 gửi một Text Frame đặc biệt qua WebSocket: `{"action": "end_of_speech"}` để báo cho Server biết đã hết câu.
5. **Server xử lý (`D:\arduino\python\server.py`):**
   - Server gom toàn bộ luồng Binary từ nãy đến giờ thành file Audio trong RAM.
   - Khi nhận được `end_of_speech`, Server ném audio vào Speech-to-Text -> Gemini -> TTS.
   - Server gửi trả về Robot:
     - Gửi JSON Text Frame (chứa `text` và `emotion`).
     - Lập tức chia nhỏ file TTS (PCM) và stream ngược lại cho ESP32 bằng Binary Frames.
6. **Robot phát âm thanh:**
   - Nhận được JSON: Chuyển khuôn mặt OLED theo cảm xúc.
   - Nhận được Binary Frames: Đẩy thẳng vào loa MAX98357 phát ra ngay lập tức mà không cần đợi tải xong (Độ trễ gần như bằng 0).

## 3. Những thay đổi Code cần thiết (Phạm vi công việc)

### A. Phía ESP32 (Arduino IDE)
- **Thư viện mạng:** Cài đặt thư viện `arduinoWebSockets` (của Markus Sattler) để làm WebSocket Client. Bỏ thư viện `HTTPClient`.
- **Core/VAD:** Sửa `Application::loop()` thành vòng lặp đọc Audio liên tục. Thêm hàm tính RMS: `sqrt(sum(sample^2) / n)`.
- **Network/Streaming:** Xóa class `HttpClient`, tạo class `WebSocketClient`. Hàm `sendAudio()` chuyển thành `streamAudioChunk()`.
- **Audio Output:** Tối ưu hóa `AudioPlayer` để nhận từng chunk nhỏ từ WebSocket và nạp vào DMA I2S.

### B. Phía Python Server (`server.py`)
- **Framework:** Cần đổi từ `Flask` sang thư viện hỗ trợ WebSocket mạnh mẽ hơn như `FastAPI` + `websockets` hoặc thêm `Flask-SocketIO` / `simple-websocket`.
- **Luồng nhận:** Cần một Async Coroutine để liên tục nhận dữ liệu `bytes` từ ESP32 và đẩy vào `BytesIO` buffer.
- **Luồng xử lý:** Khi nhận được message "stop", xử lý dữ liệu trong buffer.
- **Luồng gửi:** Gửi JSON Text (`await websocket.send(json)`), sau đó đọc file PCM và gửi bằng vòng lặp `await websocket.send(chunk_binary)`.

## 4. Kết luận

Chuyển sang WebSocket + VAD là bước tiến lớn giúp Robot biến thành một "Trợ lý ảo thực thụ" như Alexa hay Google Home (gọi và nói liên tục không cần chạm). 

Tuy nhiên, nó đòi hỏi phải viết lại hoàn toàn cơ chế mạng (`HttpClient` -> `WebSockets`) và cơ chế Audio (Block -> Non-blocking Stream). Nếu bạn đồng ý với đề xuất này, bước tiếp theo chúng ta sẽ tiến hành cấu trúc lại file `server.py` sang dạng WebSocket Server trước.
