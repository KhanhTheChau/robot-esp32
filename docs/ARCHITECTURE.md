# Kiến trúc Hệ thống (System Architecture)

Hệ thống Robot ESP32 được thiết kế dựa trên kiến trúc hướng đối tượng (OOP), sử dụng Dependency Injection để truyền các module vào `Application` core, giúp dễ dàng mở rộng và test.

## 1. Vòng đời ứng dụng (App Lifecycle)

Toàn bộ logic chính nằm trong `src/core/Application.cpp`. Trạng thái của Robot hoạt động theo một **State Machine (Máy trạng thái)** cơ bản:

- **IDLE (Nghỉ ngơi):** Robot chờ người dùng tương tác. Nó liên tục kiểm tra trạng thái nút nhấn. Màn hình hiển thị "Ready! Hold BOOT".
- **RECORDING (Đang ghi âm):** Khi người dùng **nhấn và giữ** nút (Button), Robot chuyển sang trạng thái này. Micro thu âm qua I2S và lưu vào bộ đệm (Buffer). Màn hình hiển thị hiệu ứng nhấp nháy "Recording [*]".
- **PROCESSING (Đang xử lý):** Khi người dùng **thả nút**, Robot dừng ghi âm và khóa trạng thái thành PROCESSING.
  1. Gửi toàn bộ dữ liệu âm thanh qua HTTP POST lên Server.
  2. Màn hình hiển thị "Uploading...".
  3. Nhờ Server xử lý và nhận về kết quả JSON.
  4. Hiển thị thông tin Text (câu người dùng nói / câu AI trả lời) lên OLED.
  5. Đọc URL âm thanh từ JSON và tiến hành stream trực tiếp (Download & Play) ra loa.
  6. Sau khi phát xong, nghỉ 3 giây rồi quay lại trạng thái `IDLE`.

## 2. Các Module chính (Cấu trúc thư mục `src`)

- `core/`: Chứa `Application` (trái tim của hệ thống điều phối các states) và `Logger` (in log ra Serial).
- `config/`: Chứa `AppConfig.h` lưu tất cả các thiết lập phần cứng (Chân Pin) và phần mềm (URL, Baudrate).
- `audio/`: Quản lý In/Out âm thanh. `AudioRecorder` ghi âm từ I2S Mic, `AudioPlayer` phát âm thanh ra I2S Speaker.
- `button/`: Quản lý nút nhấn vật lý chống dội (debounce).
- `display/`: Quản lý màn hình OLED.
- `network/`: `HttpClient` quản lý gửi nhận HTTP requests (POST audio, GET stream).
- `services/`: Lớp trung gian mức cao. Ví dụ `VoiceService` kết nối `AudioRecorder`, `AudioPlayer`, và `HttpClient` lại với nhau để tạo thành quy trình: Ghi âm -> Gửi lên mạng -> Nhận link -> Phát link.
- `models/`: Định nghĩa các cấu trúc dữ liệu (`VoiceResult` chứa JSON map).
- `wifi/`: Quản lý kết nối WiFi.

## 3. Dependency Injection (DI)

Robot ESP32 sử dụng interfaces (`IHttpClient`, `IDisplay`, `IAudioRecorder`, v.v.) thay vì trực tiếp sử dụng class cụ thể. Trong file chính (`main.cpp` - không nằm trong thư mục này nhưng ở cấp cao nhất của project Arduino), chúng ta sẽ khởi tạo các đối tượng và truyền vào `Application`:

```cpp
// Ví dụ mô phỏng cách main.cpp hoạt động
WiFiManager wifi;
DisplayManager display;
ButtonManager button;
AudioRecorder recorder;
AudioPlayer player;
HttpClient httpClient(logger, wifi);
VoiceService voice(logger, recorder, player, httpClient);

Application app(logger, wifi, display, button, voice);
app.begin();
```

Thiết kế này giúp bạn dễ dàng thay đổi ví dụ từ Màn hình OLED sang Màn hình TFT mà không cần sửa code trong `Application` (chỉ cần tạo class `TftDisplay` kế thừa `IDisplay`).
