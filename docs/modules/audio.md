# Module Audio (Âm thanh)

Thư mục: `src/audio/`

Chức năng: Quản lý việc thu âm thanh từ Microphone và phát âm thanh ra Loa sử dụng giao thức I2S của ESP32.

## 1. IAudioRecorder & AudioRecorder (Ghi âm)
- Interface `IAudioRecorder` chuẩn hóa các hàm: `begin()`, `startRecording()`, `stopRecording()`, `getAudioBuffer()`, `getAudioBufferSize()`.
- Lớp `AudioRecorder` sử dụng Driver I2S của ESP32 (thư viện `driver/i2s.h`).
- **Hoạt động:** Khi gọi `startRecording()`, ESP32 sẽ cấu hình I2S Bus đọc dữ liệu từ I2S Mic (INMP441). Dữ liệu này được lưu vào một mảng bộ đệm (Buffer) trên RAM của ESP32. Kích thước Buffer này phải được giới hạn cẩn thận tránh tràn RAM. Kích thước hiện tại cần được quản lý động hoặc cấp phát sẵn tĩnh.
- **Đầu ra:** Âm thanh thô dạng Raw PCM. 

## 2. IAudioPlayer & AudioPlayer (Phát loa)
- Interface `IAudioPlayer` chuẩn hóa các hàm: `begin()`, `write()`.
- Lớp `AudioPlayer` cũng sử dụng Driver I2S để đẩy dữ liệu ra I2S Amplifier (MAX98357).
- **Hoạt động:** Hàm `write(const uint8_t* data, size_t size)` nhận dữ liệu âm thanh stream từ Network và đẩy trực tiếp vào I2S port để loa phát ra. Việc này có thể thực hiện theo từng chunk nhỏ (VD: 512 bytes một lần) giúp không bị tràn RAM khi phát file audio dài (Streaming).
