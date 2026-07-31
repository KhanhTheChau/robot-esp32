# Tích hợp Khuôn mặt Cảm xúc (Emotion Faces) lên OLED

Thay vì chỉ hiển thị các dòng text đơn điệu, Robot sẽ hiển thị các khuôn mặt có cảm xúc tương ứng với nội dung câu trả lời. 

## 1. Các Cảm xúc (Emotions) Quy định

Dựa trên nghiên cứu về biểu cảm robot (tham khảo dự án mã nguồn mở [Arduino OLED Eyes](https://github.com/dhrubasaha08/OLED-Face) hoặc [Kunkune Animating Eyes](https://kunkune.co.uk/blog/2023/07/04/animating-eyes-on-a-0-96-oled-display-with-arduino/)), chúng ta sẽ quy định 5 trạng thái cảm xúc cơ bản để AI phân loại:

1. `neutral` (Bình thường): Mắt tròn, miệng nằm ngang.
2. `happy` (Vui vẻ): Mắt cong hình vòng cung (cười), miệng cong lên.
3. `sad` (Buồn): Mắt cụp xuống, miệng cong xuống.
4. `angry` (Tức giận): Mắt có lông mày nhíu lại (đường chéo), miệng ngang hoặc zíc zắc.
5. `surprised` (Ngạc nhiên): Mắt mở to tròn, miệng hình chữ 'O' hoặc elip.

## 2. Quy trình Hoạt động

1. **Phía Server (Python)**: Gemini AI sẽ nhận câu hỏi của người dùng và sinh ra câu trả lời **kèm theo cảm xúc**. Server đọc cảm xúc này và gộp vào cục JSON API (`"emotion": "happy"`).
2. **Phía Robot (ESP32)**:
   - Module `HttpClient` sẽ deserialize trường `"emotion"` từ JSON.
   - Module `DisplayManager` sẽ không gọi `printText` để in kết quả nữa, mà sẽ gọi hàm `drawFace(emotion)` để vẽ đồ họa hình học (hình tròn, elip, đường thẳng) lên OLED.

## 3. Cập nhật Prompt cho Gemini AI (Server)

Để Gemini trả về đúng cảm xúc, bạn cần cập nhật `prompt` trong `D:\arduino\python\server.py` như sau:

```python
        # Yêu cầu Gemini trả về JSON để parse lấy emotion
        prompt = f"""Bạn là một trợ lý AI nhúng trên robot. Hãy trả lời câu hỏi sau: "{text}"
Yêu cầu bắt buộc:
1. Trả lời thật ngắn gọn (dưới 15 từ).
2. Phân tích cảm xúc trong câu trả lời của bạn.
3. CHỈ trả về đúng 1 chuỗi JSON hợp lệ (không kèm markdown ```json), với định dạng:
{{
  "text": "Câu trả lời của bạn",
  "emotion": "chọn 1 trong [neutral, happy, sad, angry, surprised]"
}}"""
        
        response = model.generate_content(prompt)
        ai_response_text = response.text.strip()
        
        import json
        try:
            # Parse JSON từ Gemini
            gemini_data = json.loads(ai_response_text)
            ai_text = gemini_data.get("text", "Lỗi phản hồi")
            ai_emotion = gemini_data.get("emotion", "neutral")
        except:
            ai_text = ai_response_text
            ai_emotion = "neutral"
```

Khi đó JSON Response cuối cùng của Server trả về cho Robot sẽ là:
```json
{
    "success": true,
    "text": "Chào bạn, mình rất vui!",
    "intent": "chào robot",
    "confidence": 1.0,
    "audio_url": "http://...",
    "emotion": "happy"
}
```

## 4. Cách hiển thị đồ họa trên ESP32 (DisplayManager)

Thư viện `Adafruit_GFX` cung cấp các hàm vẽ cơ bản:
- `display.fillCircle(x, y, r, color)`: Vẽ mắt (tròn).
- `display.drawLine(x0, y0, x1, y1, color)`: Vẽ lông mày hoặc miệng thẳng.
- `display.drawPixel(x, y, color)` hoặc các nét vẽ mảnh để tạo độ cong.

**Ví dụ mã giả vẽ mắt vui vẻ (Happy):**
```cpp
// Mắt nhắm cong lên
display.drawCircleHelper(32, 20, 10, 1, WHITE); // Nửa trên bên trái
display.drawCircleHelper(96, 20, 10, 1, WHITE); // Nửa trên bên phải
// Miệng cười
display.drawCircleHelper(64, 40, 15, 2, WHITE); // Nửa dưới
```
