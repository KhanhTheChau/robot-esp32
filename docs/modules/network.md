# Module Network (Mạng & Giao tiếp HTTP)

Thư mục: `src/network/`

Chức năng: Giao tiếp với API Server thông qua giao thức HTTP/HTTPS.

## 1. IHttpClient & HttpClient
- Nằm trong thư mục `src/network`. Chịu trách nhiệm hoàn toàn về các tác vụ HTTP, giúp tách biệt logic mạng ra khỏi logic điều khiển.

### Upload Audio (`sendAudio`)
- **Nhiệm vụ:** Đẩy buffer âm thanh (Raw PCM) lên Server bằng một `POST Request`.
- Sử dụng class `HTTPClient` của lõi ESP32 (`#include <HTTPClient.h>`).
- Set timeout khá dài (ví dụ 20s) bằng `http.setTimeout(20000)` để đợi Server xử lý AI (STT -> LLM -> TTS).
- Set header: `Content-Type: application/octet-stream`.
- Trả về mã HTTP Code. Nếu HTTP Code là 200, tiến hành trích xuất String payload.

### Parse Response (`parseResponse`)
- **Nhiệm vụ:** Giải mã chuỗi JSON Server trả về (Payload) thành Object C++ (`VoiceResult`).
- Sử dụng thư viện **ArduinoJson** (class `DynamicJsonDocument`).
- Ánh xạ các trường JSON (`success`, `text`, `intent`, `confidence`, `audio_url`) vào biến thuộc class `VoiceResult`.

### Download Audio Stream (`downloadAudioStream`)
- **Nhiệm vụ:** Thực hiện `GET Request` tới `audio_url` mà Server trả về.
- Sử dụng callback `std::function` để xử lý dữ liệu tải về. Thay vì tải toàn bộ file MP3/WAV vào RAM (điều không thể do RAM ESP32 rất nhỏ), hàm này đọc từng đoạn nhỏ (chunk) từ `WiFiClient` (stream) và gọi callback. Callback này sẽ đưa chunk đó thẳng sang `AudioPlayer` để phát ra loa, quá trình này lặp lại liên tục cho đến khi tải hết file.
