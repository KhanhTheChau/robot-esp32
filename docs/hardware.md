# Cấu hình Phần cứng (Hardware Configuration)

Mọi cấu hình liên quan đến các chân cắm (Pins) và thông số kỹ thuật được tập trung duy nhất tại file `src/config/AppConfig.h`. Khi làm việc với bo mạch thật (đặc biệt là ESP32-S3), hãy đối chiếu các chân cắm ở đây.

## Sơ đồ chân (Pinout Mapping)

### 1. Nút nhấn (Button)
- Sử dụng nút BOOT có sẵn trên board ESP32-S3.
- `BUTTON_PIN = 0`

### 2. Microphone (INMP441) - Giao tiếp I2S
- `I2S_SCK_PIN = 16` (BCLK - Bit Clock)
- `I2S_WS_PIN = 17` (LRCLK - Word Select / Left Right Clock)
- `I2S_SD_PIN = 18` (DOUT - Data Out)
- `I2S_SAMPLE_RATE = 16000` (Tần số lấy mẫu 16kHz)

### 3. Speaker / Amp (MAX98357) - Giao tiếp I2S
- `I2S_SPK_BCLK_PIN = 4` (BCLK)
- `I2S_SPK_LRC_PIN = 5` (LRC)
- `I2S_SPK_DIN_PIN = 6` (DIN / Data In)

### 4. Màn hình OLED (SSD1306) - Giao tiếp I2C
- `I2C_SDA_PIN = 8` (SDA)
- `I2C_SCL_PIN = 9` (SCL)

## Lưu ý về Nguồn (Power)
Microphone và Speaker đều cần nguồn. Hãy đảm bảo bạn cắm vào chân `3.3V` hoặc `5V` (tùy spec của module rời) một cách chính xác. Mạch amply loa (MAX98357) có thể tiêu thụ dòng khá lớn khi phát tiếng lớn, nếu ESP32 bị reset khi phát nhạc, bạn cần cấp nguồn rời ổn định hơn cho module loa.
