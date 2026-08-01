# Hệ thống Hiển thị & Hoạt ảnh (Display & UI)

Robot sử dụng màn hình **OLED SSD1306 (128x64px, 1-bit)** giao tiếp qua I2C. Tuy là màn hình đơn sắc đơn giản, nhưng qua bàn tay lập trình đồ họa và quản lý bộ nhớ, nó trở nên vô cùng sống động. 

Hệ thống hiển thị có 2 tính năng chính:

---

## 1. Tính năng Ảnh động (GIF Animations) khi nghỉ (IDLE)

Tính năng này giúp màn hình OLED hiển thị mượt mà các ảnh động (nhân vật, biểu cảm) trong khoảng thời gian Robot rảnh rỗi (Trạng thái `IDLE`), thay vì chỉ hiển thị các đoạn text tĩnh đơn điệu.

Vì ESP32 và màn hình SSD1306 không hỗ trợ đọc trực tiếp định dạng file `.gif`, toàn bộ file GIF phải được chuyển đổi thành mảng byte (C array) và lưu trữ thẳng vào bộ nhớ Flash của ESP32 thông qua từ khóa `PROGMEM`.

### A. Công cụ chuyển đổi `convert_gif.py`
Nằm ở thư mục gốc của project, script này dùng thư viện `Pillow` để:
- Tách từng khung hình (frames) của file GIF.
- Resize ảnh về kích thước chuẩn của OLED: `128x64`.
- Chuyển màu thành dạng đơn sắc 1-bit (Trắng / Đen).
- Mã hóa từng pixel thành mảng mã Hex.
- Ghi toàn bộ dữ liệu ra file C++ (`src/display/GifFrames.h`).

**Lệnh chạy:**
```bash
python convert_gif.py "src/images/<ten_file>.gif" "src/display/GifFrames.h"
```

### B. Lưu trữ qua `PROGMEM` & Hoạt động
Từ khóa `PROGMEM` ép trình biên dịch (Compiler) đưa mảng dữ liệu khổng lồ của GIF vào ổ cứng (bộ nhớ Flash) thay vì nhét lên bộ nhớ RAM. Nhờ đó, ESP32 không bị tràn RAM (Out of Memory).

Trong `Application::loop()`, hàm `playGifFrame()` được thiết kế dưới dạng **Non-blocking**. Cứ mỗi 50ms, nó sẽ tự động vẽ một Frame tiếp theo lên màn hình. Nhờ vậy, ảnh động sẽ nhảy múa mượt mà ở tốc độ 20fps mà robot vẫn có thể tính toán VAD (RMS) ở chế độ ngầm mà không bị điếc.

---

## 2. Tính năng Khuôn mặt Cảm xúc (Emotion Faces) khi AI trả lời

Thay vì chỉ in câu chữ khô khan, khi AI (Gemini) bắt đầu phản hồi, Robot sẽ vẽ các khuôn mặt hình học đại diện cho cảm xúc tương ứng với nội dung câu trả lời.

### A. Quy định các Cảm xúc
Hệ thống quy định 5 trạng thái cảm xúc cơ bản:
1. `neutral` (Bình thường): Mắt tròn, miệng nằm ngang.
2. `happy` (Vui vẻ): Mắt cong hình vòng cung (cười), miệng cong lên.
3. `sad` (Buồn): Mắt cụp xuống, miệng cong xuống.
4. `angry` (Tức giận): Mắt có lông mày nhíu lại (đường chéo), miệng ngang hoặc zíc zắc.
5. `surprised` (Ngạc nhiên): Mắt mở to tròn, miệng hình chữ 'O' hoặc elip.

### B. Luồng hoạt động (Data flow)
1. **Phía Server (Python)**: Thông qua System Prompt, Server yêu cầu Gemini phân tích câu trả lời và ép nó trả về định dạng JSON chứa trường `"emotion"`.
2. **Phía Robot (ESP32)**:
   - Module `WebSocketManager` sẽ bắt JSON này và parse ra.
   - Khi chuyển sang trạng thái `SPEAKING`, `DisplayManager` sẽ không gọi `playGifFrame` nữa, mà sẽ gọi hàm `drawFace(emotion)`.
   - Hàm `drawFace` sử dụng bộ công cụ vẽ cơ bản của thư viện `Adafruit_GFX` (như `fillCircle`, `drawLine`, `drawCircleHelper`) để ráp nên khuôn mặt tức thì.

**Ví dụ mã vẽ mắt vui vẻ (Happy):**
```cpp
// Mắt nhắm cong lên (vẽ nửa đường tròn trên)
display.drawCircleHelper(32, 20, 10, 1, WHITE); // Trái
display.drawCircleHelper(96, 20, 10, 1, WHITE); // Phải
// Miệng cười
display.drawCircleHelper(64, 40, 15, 2, WHITE);
```
