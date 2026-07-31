# Giao tiếp API Server

Tài liệu này dành cho các bạn Backend Developer (phát triển Server API) để hiểu rõ cách Robot giao tiếp với Server.

## 1. Thông tin Endpoint

- **Mặc định (có thể đổi trong `src/config/AppConfig.h`):** `http://<IP_MAY_TINH>:5000/upload`
- **Method:** `POST`
- **Timeout của Robot:** 20 giây (Server cần xử lý AI và trả về kết quả trong thời gian này).

## 2. Request từ Robot (Upload Audio)

Khi người dùng nhấn và giữ nút BOOT trên Robot, Robot sẽ ghi âm giọng nói. Khi thả nút ra, Robot sẽ gửi toàn bộ dữ liệu ghi âm này lên Server.

- **Content-Type:** `application/octet-stream` (Dữ liệu nhị phân - Raw binary)
- **Định dạng Audio:** Raw PCM, Sample rate: 16000Hz, 16-bit, Mono (Tùy thuộc vào cấu hình I2S mic trên ESP32).
- **Body:** Toàn bộ buffer âm thanh ghi được.

*Lưu ý cho backend:* Bạn có thể cần convert raw PCM stream này sang dạng WAV trước khi đưa vào các mô hình AI (như Whisper, Google Speech-to-Text).

## 3. Response từ Server (JSON)

Sau khi nhận và xử lý xong âm thanh (Chuyển giọng nói thành văn bản, gọi LLM tạo phản hồi, và tạo file âm thanh phản hồi (TTS)), Server cần trả về một JSON có định dạng chuẩn xác như sau:

```json
{
  "success": true,
  "text": "Câu trả lời của AI dành cho người dùng (ví dụ: Chào bạn, tôi có thể giúp gì?)",
  "intent": "Mục đích/Ý định nhận diện được (ví dụ: greeting, turn_on_light)",
  "confidence": 0.95,
  "audio_url": "http://<IP_MAY_TINH>:5000/audio/response_123.wav"
}
```

### Các trường dữ liệu:

- `success` (boolean): Bắt buộc. Báo hiệu việc xử lý có thành công hay không. Nếu `false`, Robot sẽ báo lỗi.
- `text` (string): Nội dung văn bản AI phản hồi. Robot sẽ hiển thị nội dung này lên màn hình OLED.
- `intent` (string): (Tùy chọn) Intent nhận diện được, cũng được in ra OLED.
- `confidence` (float): (Tùy chọn) Độ tin cậy của việc nhận diện (từ 0.0 đến 1.0).
- `audio_url` (string): **Rất quan trọng**. Đường dẫn HTTP(S) đến file âm thanh (TTS) mà Server vừa tạo ra. Robot sẽ tiến hành tải stream audio từ link này và phát ra loa ngoài.

## 4. Quá trình Robot phát âm thanh (Download Audio Stream)

- Ngay sau khi nhận được JSON và có `audio_url`, Robot sẽ thực hiện một lệnh `GET` request tới `audio_url` đó.
- Server cần host file audio này và cho phép tải về (stream).
- Định dạng audio khuyên dùng cho loa của ESP32: MP3 hoặc WAV (tùy thuộc vào khả năng decode của thư viện `IAudioPlayer` đang dùng trên Robot, thường là I2S output).

## 5. Xử lý Lỗi

Nếu Server trả về HTTP Code khác `200 OK` (ví dụ `500 Server Error` hoặc `404 Not Found`), Robot sẽ coi như thất bại và in lỗi lên màn hình OLED.
