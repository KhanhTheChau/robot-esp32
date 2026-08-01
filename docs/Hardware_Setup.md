# Hướng dẫn Lắp ráp Phần cứng (Hardware Setup)

Mọi cấu hình liên quan đến các chân cắm (Pins) và thông số kỹ thuật được tập trung duy nhất tại file `src/config/AppConfig.h`. Phiên bản hiện tại được tối ưu hóa cho board **ESP32-S3**.

## 1. Sơ đồ chân cắm (Pinout Mapping)

### A. Microphone (INMP441) - Giao tiếp I2S 0
Chịu trách nhiệm thu âm thanh môi trường gửi lên Server.
- `SCK` -> `GPIO 16` (BCLK - Bit Clock)
- `WS`  -> `GPIO 17` (LRCLK - Word Select / Left Right Clock)
- `SD`  -> `GPIO 18` (DOUT - Data Out)
- `L/R` -> `GND` (Cấu hình ghi kênh Trái - Left Channel)
- `VDD` -> `3.3V`
- `GND` -> `GND`

### B. Amply Loa (MAX98357A) - Giao tiếp I2S 1
Chịu trách nhiệm khuếch đại âm thanh nhận từ Server ra loa 3W.
- `BCLK` -> `GPIO 4`
- `LRC`  -> `GPIO 5`
- `DIN`  -> `GPIO 6`
- `VIN`  -> `5V` (Hoặc `3.3V`, nhưng 5V sẽ cho âm lượng lớn hơn)
- `GND`  -> `GND`

### C. Màn hình OLED (SSD1306 128x64) - Giao tiếp I2C
Hiển thị khuôn mặt cảm xúc và trạng thái mạng.
- `SDA` -> `GPIO 8`
- `SCL` -> `GPIO 9`
- `VCC` -> `3.3V`
- `GND` -> `GND`

### D. Nút nhấn dự phòng (Button)
Mặc dù hệ thống đã hoạt động hoàn toàn Rảnh tay (Hands-free) qua VAD, nút BOOT vẫn được giữ lại trong kiến trúc `ButtonManager` để có thể sử dụng làm nút Reset hoặc ép thu âm thủ công nếu cần.
- `BUTTON_PIN` = `0` (Nút BOOT có sẵn trên board ESP32-S3).

## 2. Các Lưu ý Quan trọng về Nguồn & Chống nhiễu

### Nguồn điện (Power Supply)
Mạch amply loa (MAX98357) tiêu thụ dòng cực lớn (có thể vọt lên 500mA - 1A) mỗi khi phát tiếng bass lớn. Nếu cắm chung nguồn 3.3V của ESP32 với màn hình và loa:
- Khi Robot vừa nói, điện áp sụt giảm làm màn hình OLED bị chớp tắt, hoặc ESP32 bị sập nguồn (Brownout Reset).
- **Khắc phục:** Nên nối nguồn `VIN` của MAX98357 vào chân `5V` (VBUS/VIN) của ESP32 để dùng chung điện từ cổng USB, thay vì lấy từ bộ giảm áp 3.3V của ESP32.

### Chống nhiễu tín hiệu Âm thanh (Ticking/Static Noise)
- **Nhiễu WiFi:** Khi gửi/nhận tín hiệu âm thanh liên tục qua WebSocket, ăng-ten WiFi hoạt động công suất tối đa. Sóng RF sẽ gây nhiễu lên các dây tín hiệu I2S (hiện tượng kêu tạch tạch như máy bay trực thăng). Hãy để INMP441 tránh xa phần đầu ăng-ten của ESP32, bọc giấy bạc quanh dây tín hiệu (có nối GND) nếu cần, và cắt ngắn dây tối đa.
- **Nhiễu nguồn:** Gắn thêm một tụ hóa `10uF - 100uF` song song với 2 chân VDD và GND của module INMP441 để dập tắt nhiễu điện áp, giúp âm thanh thu vào trong trẻo hơn.
