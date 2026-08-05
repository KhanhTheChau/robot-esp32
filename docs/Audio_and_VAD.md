# Báo cáo Kỹ thuật: Cơ chế Cảm biến Giọng nói (VAD) & Xử lý Âm thanh trên Robot ESP32

Tài liệu này cung cấp báo cáo kỹ thuật toàn diện về thuật toán **Voice Activity Detection (VAD)** đang được triển khai trên Robot Myka. Báo cáo bao gồm cơ sở lý thuyết, các giải pháp kỹ thuật, hệ quy chiếu từ các sản phẩm công nghiệp thực tế và các đánh giá về bản quyền pháp lý (License) để phục vụ cho mục đích trình bày và lưu trữ dự án.

---

## 1. Giới thiệu Tổng quan về VAD

**Voice Activity Detection (VAD)** là công nghệ cốt lõi trong xử lý tín hiệu số (DSP), đóng vai trò phân tách tín hiệu giọng nói của con người khỏi các nhiễu nền (Background Noise) và khoảng lặng (Silence). Trong hệ sinh thái của Robot Myka, VAD là "người gác cổng" bắt buộc phải có nhằm đáp ứng 3 mục tiêu chiến lược:

- **Tối ưu Băng thông & Năng lượng:** Chỉ truyền phát (stream) dữ liệu âm thanh qua WiFi khi thực sự có người nói, giúp giảm tải cho module WiFi của ESP32 và tiết kiệm pin năng lượng.
- **Tối ưu Chi phí Đám mây (Cloud API Cost):** Việc loại bỏ các đoạn ghi âm tĩnh lặng giúp giảm đáng kể chi phí token/thời lượng khi gọi các API nhận diện giọng nói (như Google Speech-to-Text).
- **Bảo mật & Quyền riêng tư (Privacy):** Đảm bảo micro không liên tục truyền dữ liệu sinh hoạt của người dùng ra môi trường mạng.

---

## 2. Giải pháp VAD Cốt lõi: Năng lượng RMS

Để đáp ứng các ràng buộc khắt khe về vi xử lý (ESP32 có RAM hạn chế, không có NPU AI chuyên dụng), Robot đang sử dụng phương pháp **VAD dựa trên Năng lượng Tín hiệu (Energy-based VAD)**, đo lường qua đại lượng **RMS (Root Mean Square - Căn bậc hai trung bình bình phương)**.

### A. Cơ sở Toán học
Tín hiệu âm thanh dạng sóng (PCM) được thu thập từ micro I2S dưới dạng các mẫu số. Thuật toán RMS tính toán "độ lớn năng lượng" của một khung âm thanh (chunk) bằng công thức:

$RMS = \sqrt{\frac{1}{N} \sum_{i=1}^{N} x_i^2}$

*(Trong đó: $N$ là số lượng mẫu trong khung, $x_i$ là biên độ của mẫu thứ $i$)*

Hệ thống liên tục đối chiếu: Nếu `RMS > VAD_RMS_THRESHOLD` (ngưỡng tĩnh), vi điều khiển sẽ đánh thức luồng gửi dữ liệu.

### B. Tham chiếu Ứng dụng Thực tế trong Công nghiệp
Mặc dù là một thuật toán đơn giản, Energy-based VAD (RMS) là **tiêu chuẩn công nghiệp phổ biến nhất** trong tầng phần cứng (Hardware Layer) của các thiết bị IoT:
- **Thiết bị Smart Speaker (Amazon Echo, Google Home):** Trước khi âm thanh được đưa vào vi xử lý AI mạnh mẽ để nhận diện Wake-word, luôn có một mạch DSP cực nhỏ sử dụng thuật toán tính năng lượng (tương tự RMS) để "canh gác". Nếu môi trường quá tĩnh lặng, mạch AI sẽ ở trạng thái ngủ để tiết kiệm điện.
- **Hệ sinh thái Arduino & Espressif:** Thư viện `Arduino Sound` chính thức và các framework âm thanh của Espressif (ESP-ADF) đều tích hợp sẵn các ví dụ dùng RMS để làm chỉ thị năng lượng (Audio Metering) và kích hoạt ghi âm.
- **Phần mềm mã nguồn mở:** Các thư viện thu âm kinh điển trên Python như `SpeechRecognition` (hàm `energy_threshold`) cũng sử dụng chung nguyên lý đo năng lượng RMS để phân định sự im lặng.

### C. Đánh giá Bản quyền (Copyright & License)
- **Bản quyền thuật toán:** Công thức RMS là một kiến thức toán học cơ bản thuộc phạm vi **Public Domain (Tài sản công)**. Việc triển khai code tính toán RMS bằng C++ hoàn toàn không vi phạm bất kỳ bằng sáng chế (Patent) hay bản quyền nào. Dự án được phép tự do ứng dụng cho mục đích thương mại mà không chịu rủi ro pháp lý.

---

## 3. Bản Đồ Phát triển (Roadmap): Các Phương án VAD Tiên tiến

Khi dự án mở rộng lên các nền tảng có năng lực tính toán tốt hơn (như Raspberry Pi hoặc xử lý trên Server Python), hệ thống có thể tích hợp các thuật toán VAD nâng cao để loại bỏ hoàn toàn nhiễu (tiếng vỗ tay, còi xe, tiếng quạt):

### A. WebRTC VAD (của Google)
- **Cơ chế:** Dựa trên mô hình thống kê GMM (Gaussian Mixture Models), phân tích dải tần số đặc trưng của thanh quản con người.
- **Tính ứng dụng:** Được Google dùng làm chuẩn trong các ứng dụng họp trực tuyến (Google Meet, WebRTC).
- **License:** **BSD-3-Clause** (Cho phép thương mại hóa, chỉ cần đính kèm thông báo bản quyền gốc).

### B. Silero VAD
- **Cơ chế:** Mô hình mạng nơ-ron sâu (Deep Neural Network), đem lại hiệu quả ở đẳng cấp doanh nghiệp (Enterprise-grade) với độ chính xác tuyệt đối ngay cả trong môi trường ồn ào.
- **License:** **MIT License** (Giấy phép mã nguồn mở tự do nhất, hoàn toàn phù hợp để đóng gói bán thương mại).

> **Chiến lược hiện tại:** Đặt VAD trên Server đòi hỏi ESP32 phải truyền âm thanh liên tục (Always-on Streaming) lên mạng, gây tốn băng thông và tốn điện năng. Vì thế, duy trì **RMS trên ESP32** vẫn là sự cân bằng hoàn hảo nhất về hiệu năng/chi phí.

---

## 4. Thiết kế Cỗ máy Trạng thái (State Machine) & Chống Dội âm

Quá trình thu phát trên Robot hoạt động theo mô hình **Bán song công (Half-Duplex)**.

### Cơ chế Chống Dội âm (Acoustic Isolation)
Vấn đề lớn nhất của các robot giao tiếp là việc "tự nghe chính mình" (Ví dụ: Robot phát ra tiếng, micro lại thu tiếng đó gửi lên Server, tạo thành vòng lặp vô tận).
- **Giải pháp phần mềm:** Khi Server truyền tín hiệu TTS xuống, Robot chuyển sang trạng thái `SPEAKING`.
- **Thực thi:** Gọi lệnh `recorder.pause()` ở tầng phần cứng, lập tức ngắt xung Clock I2S của micro INMP441. Micro bị "điếc chủ động".
- **Hồi phục:** Khi phát âm thanh xong, hệ thống đợi thêm một độ trễ an toàn (thường là 500ms - 1s) để dư âm trong phòng suy hao hoàn toàn, sau đó gọi `recorder.resume()`.
- **Pháp lý:** Kỹ thuật điều khiển luồng (Flow Control) Half-Duplex là kỹ thuật viễn thông chuẩn (từ thời bộ đàm vô tuyến). Nó không thuộc về bất kỳ bằng sáng chế độc quyền nào.

---

## 5. Khuyến nghị Kỹ thuật Điện tử (Hardware Recommendations)

Dù thuật toán RMS được tối ưu hóa, tính chính xác vẫn phụ thuộc lớn vào việc triệt tiêu nhiễu phần cứng (Hardware Noise Floor):
1. **Hướng Micro (Acoustics):** Micro INMP441 là dòng đa hướng thu âm qua cổng dưới. Cần thiết kế vỏ ốp (Case 3D) có khoang thoát âm chuẩn để sóng âm hội tụ, không bị cản trở vật lý làm suy hao biên độ.
2. **Nhiễu Sóng RF (Helicopter Noise):** Sóng WiFi trên ESP32 hoạt động ở cường độ dòng điện rát lớn, dễ cảm ứng điện từ sang đường truyền I2S (chân SD). **Khuyến nghị:** Dây nối I2S phải cực ngắn, xoắn đôi (Twisted Pair) và đặt xa ăng-ten WiFi.
3. **Nguồn điện (Power Ripple):** Bắt buộc phải có tụ hóa bù dòng (`10uF - 100uF`) kẹp song song ở VDD/GND của Micro để lọc sạch nhiễu nguồn từ Màn hình OLED và Amply.

---

## 6. Tổng Kết

Giải pháp VAD hiện tại của hệ thống hoàn toàn đáp ứng các tiêu chuẩn khắt khe về **hiệu năng nhúng**, **độ ổn định** và **an toàn pháp lý**. Cấu trúc mã nguồn được thiết kế mở, sẵn sàng tích hợp các module AI tiên tiến hơn trong các giai đoạn nâng cấp tiếp theo của dự án.
