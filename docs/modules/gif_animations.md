# Tính năng Ảnh động (GIF Animations) trên OLED

Tính năng này giúp màn hình OLED hiển thị mượt mà các ảnh động (nhân vật, biểu cảm) trong khoảng thời gian Robot nghỉ (Trạng thái `IDLE`), thay vì chỉ hiển thị các đoạn text tĩnh đơn điệu.

Vì ESP32 và màn hình SSD1306 (OLED 1-bit) không hỗ trợ đọc và giải mã trực tiếp định dạng file `.gif`, toàn bộ file GIF phải được chuyển đổi thành các chuỗi byte (XBM/Bitmap array) và lưu trữ thẳng vào bộ nhớ Flash của ESP32 thông qua `PROGMEM`.

## 1. Công cụ chuyển đổi `convert_gif.py`

Để tích hợp một ảnh `.gif` mới vào code, bạn cần sử dụng Script Python được đính kèm ở thư mục gốc của dự án (`convert_gif.py`).

**Chức năng của script:**
- Tách từng khung hình (frames) của file GIF.
- Resize ảnh về kích thước chuẩn của OLED: `128x64`.
- Chuyển màu thành dạng đơn sắc 1-bit (Trắng / Đen).
- Mã hóa từng pixel (MSB) thành mảng mã Hex (dạng `0xFF`, `0x00`).
- Ghi toàn bộ dữ liệu ra file `src/display/GifFrames.h`.

**Cách chạy script:**
Mở Terminal/Command Prompt tại thư mục root của project và gõ lệnh:
```bash
python convert_gif.py "src/images/<ten_file>.gif" "src/display/GifFrames.h"
```
*(Yêu cầu máy tính phải cài đặt sẵn thư viện Pillow: `pip install pillow`)*

## 2. Lưu trữ qua `PROGMEM`

File `GifFrames.h` sau khi được tạo ra sẽ chứa mã C++ tương tự như sau:
```cpp
#include <Arduino.h>
#include <pgmspace.h>

const int gif_frame_count = 48; // Tổng số khung hình

const unsigned char gif_frame_0[] PROGMEM = { 0x00, 0xFF, ... };
const unsigned char gif_frame_1[] PROGMEM = { 0x00, 0xFF, ... };
// ...

const unsigned char* const gif_frames[] PROGMEM = {
  gif_frame_0, gif_frame_1, ...
};
```
- Từ khóa `PROGMEM` yêu cầu trình biên dịch (Compiler) đưa mảng dữ liệu khổng lồ này vào ổ cứng (bộ nhớ Flash) thay vì nhét lên bộ nhớ RAM. Nếu không có lệnh này, ESP32 sẽ lập tức bị crash do tràn RAM (Out of Memory).

## 3. Hoạt động trên ESP32 (`playGifFrame`)

Trong file `src/display/DisplayManager.cpp`, chúng ta sử dụng một hàm chạy nền `playGifFrame()`:

```cpp
void DisplayManager::playGifFrame()
{
    static int currentFrame = 0;
    static unsigned long lastFrameTime = 0;
    
    // Phát ở tốc độ ~20fps (mỗi frame cách nhau 50ms)
    if (millis() - lastFrameTime > 50) 
    {
        lastFrameTime = millis();
        clear();
        
        // Quét ảnh từ PROGMEM
        display.drawBitmap(0, 0, gif_frames[currentFrame], 128, 64, SSD1306_WHITE);
        update();
        
        // Chuyển frame tiếp theo
        currentFrame++;
        if (currentFrame >= gif_frame_count) {
            currentFrame = 0; // Lặp lại từ đầu
        }
    }
}
```
**Quy trình gọi hàm:**
Hàm `playGifFrame()` được thiết kế dưới dạng Non-blocking (Không chặn luồng, không dùng `delay()`). Nó được gọi **liên tục** từ hàm `Application::loop()` trong suốt quá trình trạng thái robot đang là `IDLE`. Nhờ vậy, ảnh động sẽ nhảy múa trên màn hình mà robot vẫn không bị điếc hay lỡ nhịp nút bấm của người dùng.
