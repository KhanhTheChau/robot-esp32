# Hướng dẫn Nạp code cho Người mới bắt đầu (Beginner's Guide)

Tài liệu này dành riêng cho các bạn chưa từng sử dụng ngôn ngữ C++ hay phần mềm Arduino IDE bao giờ. Đừng lo lắng, bạn chỉ cần làm theo đúng các bước "cầm tay chỉ việc" dưới đây là có thể nạp thành công bộ não vào Robot.

---

## Bước 1: Cài đặt Arduino IDE
1. Truy cập trang chủ: [Arduino Software](https://www.arduino.cc/en/software)
2. Tải bản **Arduino IDE 2.x** mới nhất (dành cho Windows, Mac hoặc Linux).
3. Mở file tải về và bấm `Next` liên tục để cài đặt.

---

## Bước 2: Tích hợp Board ESP32 vào Arduino
Arduino mặc định chỉ hỗ trợ các board mạch cũ. Để nó nhận diện con chip hiện đại **ESP32-S3** của bạn, ta cần tải "Gói cấu hình" (Board Manager) của nhà sản xuất Espressif.

1. Mở Arduino IDE.
2. Bấm vào `File` -> `Preferences` (Hoặc phím tắt `Ctrl + ,`).
3. Nhìn xuống ô **Additional boards manager URLs**, copy và dán đường link này vào:
   ```text
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
   ```
   *(Nếu đã có link khác ở ô đó, hãy dùng dấu phẩy `,` để cách chúng ra).*
4. Bấm `OK`.
5. Tiếp theo, nhìn sang cột bên trái của Arduino IDE, bấm vào biểu tượng **Boards Manager** (Hoặc chọn menu `Tools` -> `Board` -> `Boards Manager...`).
6. Gõ chữ `esp32` vào ô tìm kiếm.
7. Tìm cái tên **esp32 by Espressif Systems** và bấm **INSTALL**. Cứ để máy tính tải (khoảng 300MB - 1GB tùy phiên bản).

---

## Bước 3: Cài đặt các Thư viện (Libraries) bắt buộc
Robot cần sử dụng các thư viện ngoài để vẽ màn hình OLED, xử lý dữ liệu JSON và kết nối mạng. 
Ở cột bên trái IDE, bấm vào biểu tượng **Library Manager** (Hình mấy cuốn sách xếp đứng) hoặc chọn menu `Sketch` -> `Include Library` -> `Manage Libraries...`. Lần lượt tìm và bấm **INSTALL** các thư viện sau:

1. Gõ `ArduinoJson` -> Cài đặt thư viện của tác giả **Benoit Blanchon**.
2. Gõ `Adafruit SSD1306` -> Cài đặt thư viện của **Adafruit** (Nếu nó hiện bảng hỏi có muốn cài thêm các thư viện phụ thuộc như `Adafruit GFX` hay `Adafruit BusIO` không, **HÃY BẤM INSTALL ALL**).
3. Gõ `WebSockets` -> Tìm đúng thư viện có mô tả "WebSockets Server and Client for Arduino" của tác giả **Markus Sattler** và cài đặt.

---

## Bước 4: Chọn Cổng kết nối (COM Port) và Loại Board
Bây giờ, hãy cắm cáp USB nối từ máy tính vào cổng **COM/USB** trên board ESP32-S3. *(Lưu ý: Dây cáp của bạn phải là cáp Truyền dữ liệu (Data cable), tránh dùng cáp sạc dởm chỉ có dây nguồn).*

1. Vào menu `Tools` -> `Board` -> Chọn `esp32` -> Chọn **ESP32S3 Dev Module**.
2. Vào menu `Tools` -> `Port` -> Chọn cổng `COM...` vừa mới xuất hiện. *(Ví dụ COM3, COM4. Nếu bạn không thấy cổng COM nào xuất hiện, máy bạn đang thiếu Driver CH340 hoặc CP2102, hãy lên mạng tìm tải "CH340 driver" cài vào là nhận).*
3. **Cấu hình đặc biệt cho ESP32-S3:** 
   Vẫn trong menu `Tools`, bạn hãy đối chiếu và chỉnh các thông số sau (nếu có khác biệt):
   - `USB CDC On Boot`: **Enabled** (Rất quan trọng để in log ra màn hình máy tính).
   - `Flash Size`: 8MB hoặc 16MB (tùy board của bạn).
   - `PSRAM`: OPI PSRAM (nếu board của bạn có N8R8).

---

## Bước 5: Cấu hình Mạng & Nạp Code (Upload)
Gần xong rồi! Code đã tải về nhưng nó không biết pass WiFi nhà bạn.
1. Tại Arduino IDE, bấm `File` -> `Open...` và trỏ đến file `robot-esp32.ino` trong dự án của chúng ta.
2. Ở cột danh sách file bên trái, tìm thư mục `src/config/`, click phải chuột vào file `Secrets.example.h` và đổi tên nó thành `Secrets.h`.
3. Mở file `Secrets.h` lên, sửa tên mạng và pass WiFi nhà bạn:
   ```cpp
   constexpr const char* WIFI_SSID = "Ten_WiFi_Nha_Ban";
   constexpr const char* WIFI_PASSWORD = "Mat_Khau_WiFi";
   ```
4. Mở tiếp file `AppConfig.h`, tìm ô `WS_API_IP` và đổi IP thành IP của máy tính đang chạy Python Server.
5. Cuối cùng, bấm nút **Mũi tên ngang (Upload)** hình vòng tròn ở góc trên cùng bên trái giao diện Arduino IDE.
6. Ngồi đợi 1-3 phút để máy tính dịch (Compile) mã và đẩy xuống ESP32. Khi bên dưới góc báo **"Done uploading"**, xin chúc mừng! Robot của bạn đã sẵn sàng!
