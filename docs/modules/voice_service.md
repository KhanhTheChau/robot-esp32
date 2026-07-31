# Module Dịch vụ (Services)

Thư mục: `src/services/`

Chức năng: Chứa các lớp nghiệp vụ trung gian, điều phối hoạt động giữa nhiều module khác (như kết hợp Network và Audio).

## VoiceService
Đóng vai trò "nhạc trưởng" cho tính năng Voice.

- Nó giữ con trỏ tham chiếu (References) tới `IAudioRecorder`, `IAudioPlayer` và `IHttpClient`.

### Các bước hoạt động:
1. `startRecording()`: Chỉ việc gọi trực tiếp lệnh ghi âm từ AudioRecorder.
2. `stopAndUpload()`:
   - Dừng ghi âm.
   - Lấy dữ liệu thô và kích thước (Size) từ AudioRecorder.
   - Trực tiếp chuyển dữ liệu đó vào hàm `sendAudio` của HttpClient.
   - Trả về đối tượng kết quả `VoiceResult`.
3. `playResponse(url)`:
   - Nhận URL từ `VoiceResult`.
   - Gọi lệnh `downloadAudioStream` trong HttpClient.
   - Truyền một Lambda (Callback function) vào. Cứ mỗi lần HttpClient tải được một gói tin Audio từ mạng, Lambda này sẽ nhận và đẩy gói tin đó qua AudioPlayer để loa phát ra ngay lập tức.
