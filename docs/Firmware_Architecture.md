# Cấu trúc Mã nguồn ESP32 (Code Structure)

Mã nguồn C++ của ESP32 được xây dựng theo triết lý **Hướng đối tượng (OOP)**, **Dependency Injection** và sử dụng **Máy trạng thái (State Machine)**.

Toàn bộ code xử lý logic cốt lõi nằm trong thư mục `src/`. Dưới đây là chức năng của từng Class.

## 1. Application (State Machine)
Nằm ở `src/core/Application.cpp`. Đây là "Tổng tư lệnh" của hệ thống. Nó duy trì vòng lặp hoạt động (loop) và quản lý 4 trạng thái chính:
- **`IDLE`**: Gọi hàm phát ảnh GIF (`playGifFrame`) và cho phép VAD lắng nghe tiếng động.
- **`LISTENING`**: Nháy chữ "Listening" trên màn hình, tiếp tục cho phép VAD gửi âm thanh lên mạng.
- **`PROCESSING`**: Gửi cờ kết thúc (end_of_speech) và chờ đợi Server phản hồi.
- **`SPEAKING`**: Chặn VAD không cho thu âm (tránh Acoustic Echo), hiển thị khuôn mặt cảm xúc (`drawFace`), và đợi quá trình phát âm thanh từ loa kết thúc (sau 8s timeout).

## 2. ConversationStateManager (Bộ Quản lý Trạng thái Hội thoại)
Nằm ở `src/core/ConversationStateManager.cpp`. Nhận trách nhiệm liên kết phần cứng Âm thanh (Audio) và Mạng (WebSocket).
- Có vòng lặp `loop()` gọi hàm đọc dữ liệu từ Micro.
- Tính toán RMS. Nếu lớn hơn ngưỡng, tự động gọi `webSocket.sendAudioChunk()`.
- Giao tiếp với Server bằng JSON (`WAKE_UP`, `THINKING`, `CHAT_RESPONSE`, `GO_TO_SLEEP`). Quản lý trạng thái `SLEEP` (chỉ kiểm tra wake word) và `AWAKE`.
- Ngắt Micro (pause VAD) khi Robot đang phát âm thanh để chống Echo.

## 3. WebSocketManager (Mạng lưới)
Nằm ở `src/network/WebSocketManager.cpp`. Quản lý toàn bộ giao tiếp 2 chiều với Server Python.
- Dùng thư viện `WebSocketsClient` của Markus Sattler.
- Bắt sự kiện `WStype_TEXT`: Giải mã JSON (bằng ArduinoJson) lấy text và cảm xúc (emotion), sau đó ném qua callback `onVoiceResult`.
- Bắt sự kiện `WStype_BIN`: Nhận luồng âm thanh PCM từ Server trả về, đẩy thẳng vào callback `onAudioStream` (nối trực tiếp với `AudioPlayer`).

## 4. AudioRecorder & AudioPlayer
Nằm ở `src/audio/`. Trực tiếp giao tiếp I2S cấp thấp.
- **`AudioRecorder`**: Giao tiếp qua `I2S_PORT_0`. Đọc non-blocking 50ms để lấy dữ liệu PCM thô từ Micro INMP441, trả về dạng byte array. Đồng thời chứa hàm tính toán thuật toán căn bậc hai trung bình `calculateRMS()`.
- **`AudioPlayer`**: Giao tiếp qua `I2S_PORT_1`. Nhận mảng byte từ WebSocket, ném vào `i2s_write` đẩy ra amply MAX98357 để phát tiếng.

## 5. Dependency Injection (Khởi tạo)
Hãy mở file `robot-esp32.ino`, bạn sẽ thấy toàn bộ các Class trên không bao giờ tự tạo ra (new) các Class khác bên trong nó. Thay vào đó, tất cả đều được tạo ra ở `setup()` và "Bơm" (Inject) vào nhau thông qua Constructor.
Kiến trúc này giúp dễ dàng thay thế phần cứng (Ví dụ: Thay màn hình SSD1306 thành TFT thì chỉ cần viết Class `TFTManager` kết thừa từ `IDisplay` và nhúng vào `Application`).
