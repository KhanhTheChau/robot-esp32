# Báo cáo Chuyên sâu: Cơ chế Cảm biến Giọng nói (VAD) & Xử lý Âm thanh trên Robot ESP32

Tài liệu này cung cấp cái nhìn toàn diện về thuật toán **VAD (Voice Activity Detection)** đang được sử dụng trong dự án, các cơ sở lý thuyết, nguồn tham khảo học thuật/mã nguồn mở, cũng như các vấn đề liên quan đến bản quyền (License). Tài liệu được chuẩn bị dưới dạng báo cáo kỹ thuật.

---

## 1. Giới thiệu về VAD (Voice Activity Detection)

**Voice Activity Detection (VAD)** là kỹ thuật phân tách tín hiệu âm thanh có chứa tiếng nói của con người (Speech) khỏi các tín hiệu nhiễu nền (Background Noise) hoặc sự im lặng (Silence). Trong các ứng dụng rảnh tay (Hands-free) như Robot Myka, VAD là thành phần không thể thiếu để:
- Tiết kiệm băng thông: Chỉ gửi dữ liệu âm thanh lên Cloud khi có tiếng người.
- Tiết kiệm API Cost: Giảm chi phí cho các dịch vụ Speech-to-Text (như Google STT) nhờ việc không gửi băng ghi âm trống.
- Đảm bảo quyền riêng tư: Micro không liên tục truyền dữ liệu ra môi trường mạng.

---

## 2. Giải pháp VAD hiện tại của Robot: Năng lượng RMS

Hiện tại, để phù hợp với giới hạn phần cứng của ESP32 (RAM hạn hẹp, không có NPU xử lý AI chuyên dụng), Robot đang sử dụng phương pháp phát hiện giọng nói dựa trên **Năng lượng tín hiệu (Energy-based VAD)** thông qua đại lượng **RMS (Root Mean Square)**.

### A. Bản chất Toán học
Tín hiệu âm thanh dạng sóng (PCM) được lấy mẫu. RMS tính toán "độ lớn" hoặc "năng lượng" trung bình của đoạn âm thanh (chunk) đó bằng công thức Căn bậc hai trung bình bình phương:
$RMS = \sqrt{\frac{1}{N} \sum_{i=1}^{N} x_i^2}$
*(Trong đó: N là số lượng mẫu, $x_i$ là biên độ của mẫu thứ i)*

Khi `RMS > VAD_RMS_THRESHOLD`, hệ thống xác định là đang có người nói.

### B. Ưu điểm và Nhược điểm
- **Ưu điểm:** Độ trễ cực thấp (O(N)), tốn rất ít CPU, cực kỳ phù hợp cho Vi điều khiển (Microcontrollers).
- **Nhược điểm:** Dễ bị đánh lừa bởi các tiếng động lớn không phải giọng người (tiếng vỗ tay, còi xe, quạt máy). Yêu cầu phải tinh chỉnh ngưỡng (Tuning) thủ công cho từng môi trường tĩnh/động khác nhau.

### C. Vấn đề Bản quyền (Copyright & License)
- **Bản quyền thuật toán:** RMS là một công thức toán học cơ bản và thuộc phạm vi **Public Domain (Tài sản công)**. Việc sử dụng, triển khai code tính toán RMS bằng C++ hoàn toàn không vi phạm bất kỳ bằng sáng chế (Patent) hay bản quyền nào. Bạn có thể sử dụng cho dự án mã nguồn mở lẫn thương mại (Commercial) một cách hợp pháp.

---

## 3. Các Phương án VAD Tiên tiến (Tham khảo & Nâng cấp)

Trong tương lai, nếu đẩy việc xử lý VAD lên Server Python hoặc sử dụng các vi điều khiển mạnh hơn (Raspberry Pi), dự án có thể tham khảo các mã nguồn mở sau để tăng tính chính xác:

### A. WebRTC VAD (của Google)
WebRTC là dự án mã nguồn mở do Google phát triển, chứa một bộ VAD rất nổi tiếng dựa trên mô hình **GMM (Gaussian Mixture Models)**. Nó phân tích các đặc trưng tần số thay vì chỉ dựa vào âm lượng.
- **Hiệu năng:** Rất nhanh, phân loại chính xác tín hiệu giọng nói (10ms, 20ms, 30ms frames).
- **Tham khảo & Github:** 
  - Source C gốc: [WebRTC GitHub Repository](https://github.com/webrtc-mirror/webrtc)
  - Python wrapper phổ biến: [py-webrtcvad](https://github.com/wiseman/py-webrtcvad)
- **Giấy phép (License):** Mã gốc của Google sử dụng **BSD-3-Clause License**. Đây là một giấy phép mã nguồn mở vô cùng tự do. Bạn được phép sử dụng, sửa đổi và dùng trong sản phẩm thương mại, với điều kiện duy nhất là phải đính kèm thông báo bản quyền của tác giả gốc trong mã nguồn của bạn.

### B. Silero VAD
Silero VAD là một mô hình mạng nơ-ron (Neural Network / Deep Learning) cực kỳ gọn nhẹ nhưng đem lại hiệu quả ở đẳng cấp doanh nghiệp (Enterprise-grade).
- **Hiệu năng:** Độ chính xác vượt trội hơn hẳn WebRTC VAD trong môi trường nhiều nhiễu, nhưng đòi hỏi tính toán nặng hơn (cần CPU tốt hoặc Server).
- **Tham khảo & Github:** [Silero VAD GitHub Repository](https://github.com/snakers4/silero-vad)
- **Giấy phép (License):** Sử dụng **MIT License**. Tương tự BSD, MIT cho phép bạn làm hầu như mọi thứ (kể cả bán thương mại đóng mã nguồn) miễn là giữ lại file License chứa tên của tác giả Silero. 

> **Nhận định cho Robot Myka:** Nếu áp dụng, ta sẽ tích hợp Silero VAD hoặc WebRTC VAD tại phía Server (Python `server.py`). Tuy nhiên, do VAD chủ yếu để "đánh thức" luồng ghi âm từ ESP32, việc đặt VAD trên Server đòi hỏi ESP32 phải truyền âm thanh liên tục (Always-on Streaming) lên mạng, gây tốn băng thông và tốn điện năng. Vì thế, RMS trên ESP32 vẫn là sự cân bằng hoàn hảo nhất hiện tại.

---

## 4. Thiết kế Luồng Thu âm I2S và Cỗ máy Trạng thái (State Machine)

Quá trình thu âm trên Robot được quản lý bởi file `core/ConversationStateManager.cpp`. Hệ thống hoạt động theo mô hình Bán song công (Half-Duplex) với các trạng thái sau:

### Trạng thái SLEEP và Lắng nghe
- Mạch micro I2S (INMP441) chạy ngầm, lấy mẫu âm thanh và cắt thành các khối 1024 bytes.
- Tính toán RMS liên tục. Khi RMS vượt ngưỡng, ESP32 sẽ gửi các đoạn Audio PCM 16-bit qua WebSocket lên Server.
- Server tiến hành phân tích STT. Nếu đúng từ khóa "Myka", Server trả JSON `{"action": "WAKE_UP"}`, đưa State Machine trên ESP32 vào chế độ `AWAKE`.

### Cơ chế Chống Dội âm (Acoustic Isolation & Echo Cancellation)
Đây là kỹ thuật giải quyết việc Robot "tự nghe chính mình" khi phát ra tiếng (Ví dụ: Robot nói "Cháu đây", micro thu lại tiếng đó và tiếp tục đưa lên AI).
- **Giải pháp phần mềm:** Khi nhận được tín hiệu âm thanh TTS từ Server để phát ra loa MAX98357, biến `_isRobotSpeaking` được bật lên `true`.
- **Thực thi:** Gọi `recorder.pause()`, I2S lập tức ngừng đọc tín hiệu, trả về mảng byte trống (0 bytes). Micro bị "điếc tạm thời".
- **Hồi phục:** Sau khi kết thúc luồng byte âm thanh cuối cùng khoảng 500ms (an toàn chờ dư âm tan hết trong phòng), `recorder.resume()` được gọi. Micro hoạt động trở lại. 
- **Đánh giá bản quyền:** Phương pháp ngắt mic luân phiên (Half-Duplex) là kỹ thuật điều khiển luồng (Flow Control) cơ bản của ngành viễn thông từ thời bộ đàm (Walkie-Talkie). Nó không chịu sự quản lý của bất kỳ bằng sáng chế nào và hoàn toàn tự do sử dụng.

---

## 5. Các yếu tố Vật lý và Điện tử ảnh hưởng đến VAD

Dù thuật toán có tối ưu đến đâu, Năng lượng RMS vẫn chịu chi phối mạnh bởi thiết kế phần cứng:

1. **Khoảng cách và Hướng Micro INMP441:** Micro đa hướng (Omnidirectional) lấy âm qua lỗ nằm ở mặt dưới (mặt không chứa linh kiện). Bịt kín mặt này sẽ làm RMS của giọng nói tiệm cận về 0.
2. **Nhiễu WiFi (Helicopter Noise):** ESP32 phát sóng RF khi truyền WebSocket. Sóng RF gắp nhiễu vào đường tín hiệu I2S (chân SD) sinh ra tiếng "tạch tạch", làm RMS bị ảo hóa tăng vọt. **Cách xử lý:** Đi dây I2S thật ngắn, xoắn dây và để xa Anten WiFi.
3. **Nguồn điện không sạch (Ripple):** Cấp chung nguồn 3.3V của Micro INMP441 với Màn hình OLED hoặc Amply sẽ tạo nhiễu nền (Noise Floor) cao. Nên có tụ hóa (`10uF - 100uF`) kẹp giữa VDD và GND của INMP441 để dập nhiễu điện áp.

---

## 6. Kết luận & Khuyến nghị Báo cáo

Trong bản báo cáo, bạn có thể tự tin khẳng định:
- Dự án sử dụng giải pháp VAD dựa trên Năng lượng (RMS), tối ưu tài nguyên mức tối đa cho nền tảng Embedded. 
- Mã nguồn VAD hiện tại (Viết bằng C++ trong `AudioRecorder.cpp`) là mã nguồn nguyên bản, thuật toán toán học cơ sở **(Public Domain)**, hoàn toàn đáp ứng các tiêu chuẩn khắt khe nhất về an toàn pháp lý và bản quyền phần mềm để thương mại hóa hoặc công bố diện rộng.
- Hệ thống hỗ trợ nâng cấp mở rộng lên AI VAD (WebRTC, Silero) với các giấy phép mã nguồn mở tiêu chuẩn quốc tế (MIT, BSD-3) khi có nhu cầu tính toán trên Edge PC hoặc Cloud.
