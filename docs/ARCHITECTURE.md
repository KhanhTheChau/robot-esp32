# Kiến trúc Hệ thống Voice Assistant (ESP32-S3 + INMP441 + Gemini)

Tài liệu này lưu trữ toàn bộ kiến thức kỹ thuật quan trọng nhất về module Voice, được tinh chỉnh sau rất nhiều lần thử nghiệm thực tế. **Đây là bản thiết kế chuẩn mực (Golden Reference) dùng để phục hồi nếu hệ thống bị lỗi trong tương lai.**

## 1. Sơ đồ luồng dữ liệu (Data Flow)

1. **Hardware (INMP441):** Thu thập âm thanh qua I2S (24-bit MSB-aligned).
2. **ESP32-S3 (AudioRecorder):** Giao tiếp I2S bằng DMA, cấu hình tự động cắt 16-bit MSB, đưa vào buffer.
3. **ESP32-S3 (HttpClient):** Nén âm thanh dạng thô (PCM 16-bit, 16kHz, Mono) và POST lên Server qua mạng WiFi.
4. **Python Server:** Nhận dữ liệu, đóng gói thành file `.wav` chuẩn để lưu trữ và nạp vào Google Speech-To-Text API.
5. **Gemini AI:** Nhận văn bản từ Google STT, sinh câu trả lời ngắn gọn (phù hợp với thiết bị nhúng).
6. **ESP32-S3 (DisplayManager):** Parse JSON trả về, render văn bản của Gemini lên màn hình OLED 0.96".

## 2. Các Bài học Kỹ thuật Sâu sắc (Tránh Lối Mòn)

### A. Lỗi xung đột Nút BOOT và Xung nhịp I2S (MCLK Bug)
- **Triệu chứng:** Khi khởi tạo `i2s_driver_install` trên ESP32-S3, mạch liên tục bị crash "Connecting..." hoặc tự động nhảy vào chế độ "Recording" dù người dùng không bấm nút.
- **Nguyên nhân:** Driver I2S mặc định của Espressif (ESP-IDF) tự động xuất tín hiệu xung nhịp Master Clock (MCLK) ra chân `GPIO 0`. Chân này lại chính là chân của nút BOOT. Tín hiệu xung nhịp cao tầng làm nhiễu tín hiệu kéo-thả của nút bấm.
- **Giải pháp dứt điểm:** Phải ép `mck_io_num` về -1 (không sử dụng).
```cpp
#if (ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0))
    pin_config.mck_io_num = I2S_PIN_NO_CHANGE;
#endif
```

### B. Lỗi Vỡ tiếng / Nhiễu tĩnh điện (Audio Clipping / Static Noise)
- **Triệu chứng:** File âm thanh thu được nghe toàn tiếng xèo xèo, nổ lụp bụp, Google STT không thể nhận dạng.
- **Phân tích:** 
  - INMP441 trả về dữ liệu 24-bit được "chèn" vào trong khung truyền 32-bit (dữ liệu nằm ở dải MSB - các bit cao nhất).
  - Lúc đầu chúng ta cố tình cấu hình I2S đọc dạng 32-bit, sau đó dùng thuật toán dịch bit thủ công (`>> 11`, `>> 13`) để giảm xuống 16-bit và tăng âm lượng. Phép toán thủ công này tạo ra hệ số nhân âm lượng khổng lồ (Gain x8, x32), khiến biên độ sóng âm thanh chạm trần (-32768 đến 32767) dẫn đến Clipping kịch liệt.
- **Giải pháp dứt điểm (Theo chuẩn DroneBotWorkshop):**
  - Không cần tính toán thủ công. Trình điều khiển DMA của ESP32 cực kỳ thông minh.
  - Cấu hình chuẩn: `bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT`. Phần cứng ESP32 sẽ tự động bóc tách đúng 16 bit MSB từ tín hiệu 24-bit của INMP441, nạp trực tiếp vào biến `int16_t` với chất lượng cực kì trong trẻo và nguyên bản (Gain 1:1).

### C. Lỗi Kênh Trái/Phải (L/R Channel)
- **Chuẩn bị:** Chân L/R của INMP441 PHẢI được nối với `GND`.
- **Cấu hình:** `channel_format = I2S_CHANNEL_FMT_ONLY_LEFT`.
- Nếu bỏ lửng chân L/R (floating), tín hiệu sẽ bị nhiễu dải điện một chiều (DC Offset) khổng lồ, khiến tín hiệu âm thanh bị ép xuống đáy -20000.

## 3. Cấu trúc Source Code ESP32

- `src/audio/AudioRecorder.cpp`: Trái tim của hệ thống I2S. Nơi thiết lập DMA và giao tiếp với Micro.
- `src/button/ButtonManager.cpp`: Xử lý chống dội phím (Debounce) cho nút bấm BOOT (GPIO 0).
- `src/network/HttpClient.cpp`: Xử lý gửi HTTP POST raw binary (PCM) tới Server Python mà không cần đóng gói Wav Header (Wav Header được ráp tại Python để tiết kiệm RAM cho ESP32).
- `src/display/DisplayManager.cpp`: Giao tiếp I2C với OLED (GPIO 11, GPIO 12).
- `src/services/VoiceService.cpp`: State Machine (Trạng thái) kết nối toàn bộ quy trình: Bấm nút -> Thu âm -> Gửi mạng -> Hiển thị kết quả.

## 4. Phục hồi và Phát triển
Nếu trong tương lai hệ thống âm thanh bị hỏng do cập nhật Core Arduino hoặc thay đổi vi điều khiển, hãy đối chiếu các thông số I2S trong `AudioRecorder.cpp` với tài liệu này. Đừng cố gắng dịch bit thủ công (Bit-shifting) trừ khi bạn thực sự phải giao tiếp bằng `I2S_BITS_PER_SAMPLE_32BIT` (chỉ dùng khi dùng các thư viện DSP chuyên sâu).

