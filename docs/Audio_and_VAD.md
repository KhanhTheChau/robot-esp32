# Hướng dẫn tinh chỉnh VAD (Cảm biến Giọng nói) & Các yếu tố ảnh hưởng đến Âm thanh

Trong kiến trúc giao tiếp "Hands-free" (Rảnh tay), Robot sử dụng thuật toán **VAD** (Voice Activity Detection) dựa trên **RMS** (Root Mean Square - Căn bậc hai trung bình bình phương) để nhận biết lúc nào bạn bắt đầu nói và lúc nào bạn kết thúc câu.

Việc chọn ngưỡng RMS (`VAD_RMS_THRESHOLD`) là một nghệ thuật, vì nếu để quá thấp, Robot sẽ liên tục tự nói chuyện một mình do nhiễu môi trường. Nếu để quá cao, bạn sẽ phải hét lên thì nó mới nghe thấy.

---

## 1. Cách đo lường và Chọn ngưỡng RMS

Thông số này được cấu hình tại file `src/config/AppConfig.h`:
```cpp
constexpr float VAD_RMS_THRESHOLD = 500.0f; // Ngưỡng kích hoạt
constexpr unsigned long VAD_SILENCE_TIMEOUT_MS = 1500; // Thời gian im lặng (ms)
```

### Quy trình "Dò sóng" (Tuning):
1. **Mở Serial Monitor:** Kết nối ESP32 với máy tính, mở Serial Monitor ở baud rate `115200`.
2. **Quan sát Noise Floor (Độ ồn nền):**
   - Đặt Robot ở môi trường hoạt động bình thường (không nói chuyện).
   - Quan sát log `[INFO] Current Audio RMS: XXX`.
   - Giả sử khi im lặng, `XXX` dao động từ `50` đến `120`. Đây gọi là độ ồn nền (Noise Floor).
3. **Thử nghiệm nói:**
   - Đứng ở khoảng cách bạn thường tương tác với Robot (VD: cách 50cm).
   - Nói một câu bình thường: "Hôm nay thời tiết thế nào?".
   - Quan sát log, lúc này RMS có thể vọt lên mức `800` - `1500`.
4. **Chốt con số:**
   - Chọn `VAD_RMS_THRESHOLD` nằm ở khoảng giữa, nhưng nghiêng về phía ồn nền một chút để dễ bắt giọng. 
   - Công thức gợi ý: `Ngưỡng = Noise_Floor_Max + 150` (Ví dụ: `120 + 150 = 270`). 
   - Đừng set quá sát (VD: 130) vì quạt máy hoặc tiếng xe cộ chạy ngang qua có thể làm Robot thức giấc nhầm.

### Thời gian ngắt câu (`VAD_SILENCE_TIMEOUT_MS`):
- Nếu bạn hay có thói quen ngập ngừng ("À... ừm..."), hãy tăng lên `2000` (2 giây).
- Nếu bạn muốn Robot phản xạ cực nhanh ngay sau khi bạn chốt câu, hãy giảm xuống `1000` hoặc `800`.

---

## 2. Các yếu tố Vật lý và Phần cứng ảnh hưởng đến Thu âm

Cường độ RMS không chỉ phụ thuộc vào phần mềm mà còn bị ảnh hưởng cực mạnh bởi phần cứng, cụ thể là module micro INMP441 và cách thiết kế mạch. Dưới đây là các bệnh thường gặp:

### A. Khoảng cách và Hướng Micro
- INMP441 là micro **Omnidirectional** (đa hướng), lỗ lấy âm nằm ở mặt dưới (mặt không có linh kiện). Nếu bạn vô tình dán kín mặt này vào khung nhựa của robot, âm thanh sẽ bị bóp nghẹt, dẫn đến RMS khi nói cực kỳ thấp.
- **Cách khắc phục:** Đảm bảo lỗ lấy âm hướng thẳng ra ngoài vỏ robot, không bị che khuất.

### B. Nhiễu WiFi (Ticking Noise / Helicopter Noise)
- ESP32 là chip có kết nối WiFi, khi truyền tải dữ liệu (stream âm thanh), ăng-ten WiFi phát ra sóng RF rất mạnh. Nếu đường dây tín hiệu I2S (đặc biệt là dây `SD` - Serial Data) nằm quá sát ăng-ten WiFi, nó sẽ sinh ra nhiễu điện từ.
- Biểu hiện: Có tiếng "tạch tạch tạch" như máy bay trực thăng trong file thu âm, làm RMS nền tăng vọt lên mức hàng ngàn một cách ảo.
- **Cách khắc phục:** 
  - Dùng dây điện ngắn nhất có thể nối từ INMP441 tới ESP32.
  - Xoắn các dây tín hiệu lại với nhau.
  - Đặt INMP441 cách xa phần đầu có chứa Ăng-ten của ESP32.

### C. Nguồn điện không sạch (Power Supply Noise)
- INMP441 dùng nguồn 3.3V. Nếu bạn cấp chung đường 3.3V này với các thiết bị ăn dòng lớn (như màn hình OLED hoặc Amply MAX98357), điện áp sẽ bị sụt giảm hoặc dao động (Ripple).
- Biểu hiện: Âm thanh bị sôi (Static noise).
- **Cách khắc phục:** Đấu thêm một tụ hóa (khoảng `10uF - 100uF`) song song giữa chân `VDD` và `GND` của INMP441 để lọc nguồn.

### D. Vấn đề "Tự nghe chính mình" (Acoustic Echo)
Bạn sẽ thắc mắc: **"Khi loa của Robot đang phát âm thanh trả lời, micro có thu lại chính âm thanh đó và tưởng là tôi đang nói không?"**

Câu trả lời là **CÓ**, nếu không có biện pháp phòng ngừa. Hiện tượng này gọi là **Acoustic Echo** (Tiếng dội âm thanh). Nếu xảy ra, Robot sẽ tự thu lại câu trả lời của chính mình, rồi đẩy lên Server STT, Server lại tạo ra câu trả lời mới, và Robot kẹt trong một "Vòng lặp vô hạn" tự nói chuyện một mình.

Để giải quyết triệt để, chúng ta có 3 hướng đi (hiện tại Project đang áp dụng Hướng 1):

#### Hướng 1: Bịt tai khi đang nói (Half-Duplex qua State Machine) - Đang áp dụng
Đây là giải pháp phần mềm đơn giản, nhẹ và hiệu quả nhất cho ESP32.
- **Nguyên lý:** Khi Robot bắt đầu nói, nó sẽ "bịt tai" lại (tạm dừng thu âm).
- **Trong Code (`Application.cpp`):** 
  - ESP32 được quản lý bằng State Machine (Máy trạng thái). Hàm `voice.loop()` (hàm chuyên lấy dữ liệu từ micro để tính RMS) CHỈ ĐƯỢC GỌI khi ở trạng thái `IDLE` (chờ đợi) hoặc `LISTENING` (đang nghe bạn nói).
  - Khi Robot chuyển sang trạng thái `SPEAKING` (đang phát âm thanh ra loa), hệ thống **tuyệt đối không gọi** `voice.loop()`. 
  - Kết quả là, toàn bộ sóng âm phát ra từ loa dù có dội vào micro cũng bị ESP32 vứt bỏ hoàn toàn. Nó trở nên "điếc tạm thời" cho đến khi nói xong.
- **Nhược điểm:** Bạn không thể "ngắt lời" (Interrupt) Robot khi nó đang nói. Bạn bắt buộc phải đợi nó nói hết câu, trạng thái trở về `IDLE` thì bạn mới nói tiếp được.

#### Hướng 2: Thiết kế chống dội Cơ học (Acoustic Isolation)
Dùng thiết kế vật lý của vỏ vỏ (Vỏ in 3D).
- Đặt Loa và Micro ở hai khoang cách biệt hoàn toàn.
- Bọc đệm cao su hoặc mút tiêu âm quanh củ loa để vách nhựa không truyền rung động (vibration) trực tiếp sang board mạch chứa micro.
- Hướng bắt âm của micro và hướng phát của loa phải quay ra hai góc ngược nhau (ví dụ: Loa ở sau lưng, Micro ở trước ngực).

#### Hướng 3: Triệt tiêu tiếng vọng Thuật toán (Full-Duplex AEC - Advance)
Nếu muốn làm tính năng "Ngắt lời Robot giống ChatGPT Voice", bạn phải để micro luôn mở (Full-Duplex).
- ESP32 không đủ sức mạnh xử lý thuật toán AEC (Acoustic Echo Cancellation) theo thời gian thực (như các thuật toán của Speex, WebRTC).
- Nếu muốn làm, bạn phải truyền liên tục sóng âm cả lúc nó đang nói lên Server. Trên Server Python, dùng một thư viện AEC (như thư viện `webrtc-aec`) để lấy luồng âm thanh gốc (luồng đang phát cho ESP32) trừ đi (subtract) luồng âm thanh thu được từ ESP32. Sóng âm nào trùng khớp sẽ bị triệt tiêu, chỉ chừa lại giọng nói của bạn xen vào. Hướng này tốn rất nhiều công sức xử lý trên Backend.

---

## 3. Các yếu tố Mềm (Software)

- **Sample Rate (Tần số lấy mẫu):** Hiện tại hệ thống dùng `16000 Hz`, đây là "Tiêu chuẩn vàng" (Golden standard) cho nhận diện giọng nói (Google STT, Whisper, v.v.). Không nên tăng lên `44100 Hz` vì nó làm lãng phí băng thông mạng, tràn RAM ESP32 mà AI lại không nhận dạng tốt hơn.
- **Bits Per Sample:** `16-bit` là mức chuẩn để phân bổ âm lượng.
- **Kích thước Buffer (`VAD_CHUNK_SIZE`):** Gửi chunk `1024 bytes` (~32ms) giúp Server xử lý liên tục không độ trễ. Gửi chunk quá to (>4096 bytes) sẽ gây khựng màn hình LED trên ESP32 vì I2S phải block lâu hơn.
