# Khái quát Kiến trúc Hệ thống

Dự án Robot ESP32 hoạt động theo mô hình Client-Server. Trong đó, phần cứng nhúng (ESP32) đóng vai trò là Client phụ trách giao tiếp vật lý, còn Python đóng vai trò Server đảm nhiệm toàn bộ sức mạnh xử lý AI, phân tích và trích xuất. Sự phân chia này giúp ESP32 chạy cực kỳ nhẹ nhàng mà vẫn mang lại cảm giác thông minh.

## 1. Sơ đồ Hoạt động Tổng quan (High-level Architecture)

Giao tiếp giữa 2 khối được thực hiện hoàn toàn qua giao thức **WebSocket** (Full-Duplex), đảm bảo độ trễ thấp nhất có thể cho một luồng hội thoại.

```mermaid
graph LR
    subgraph ESP32 [Robot - C++ Client]
        Mic[INMP441 Microphone] -->|I2S| VAD[VAD / RMS Engine]
        VAD -->|WebSocket BIN| Net[WebSocket Client]
        Net -->|I2S| Spk[MAX98357 Speaker]
        Net --> OLED[SSD1306 Display]
    end

    subgraph Server [Python WebSocket Server]
        WSS[WebSocket Server] --> STT[Google Speech-To-Text]
        STT --> LLM[Google Gemini 1.5]
        LLM --> TTS[Microsoft Edge-TTS]
        LLM --> Emotion[Emotion Extractor]
        TTS --> FFmpeg[FFmpeg PCM Streamer]
        FFmpeg -->|WebSocket BIN| WSS
        Emotion -->|WebSocket JSON| WSS
    end
    
    Net <-->|Wi-Fi / LAN| WSS
```

## 2. Các pha giao tiếp (Chu kỳ hỏi - đáp)

Thay vì thiết kế dạng nút bấm "nhấn để nói" (Push-to-talk), hệ thống được cấu trúc dựa trên **State Machine** (Máy trạng thái) hoàn toàn Rảnh tay (Hands-free). 

Chu kỳ của một cuộc hội thoại tuân theo biểu đồ tuần tự sau:

```mermaid
sequenceDiagram
    autonumber
    actor U as Người dùng
    participant E as ESP32 (VAD)
    participant S as Server (Python)
    participant A as AI Cloud (Gemini)

    Note over E,S: Khởi động - ESP32 kết nối WebSocket Server
    
    U->>E: Bắt đầu nói chuyện
    E->>E: Tính toán RMS vượt ngưỡng (VAD Trigger)
    Note over E: Chuyển State: LISTENING (VAD)
    
    loop Đang nói
        E->>S: Gửi các đoạn Audio PCM 16-bit qua WebSocket (BIN)
    end
    
    U->>E: Ngừng nói
    E->>E: Cường độ âm thanh tụt giảm dưới ngưỡng trong 1.5 giây
    Note over E: Chuyển State: PROCESSING
    E->>S: Gửi JSON báo hiệu kết thúc câu: {"action": "end_of_speech"}
    
    S->>S: Kiểm tra State. Nếu AWAKE -> Gửi action: "THINKING"
    S->>E: Gửi JSON: {"action": "THINKING"} và Stream Audio "Đang suy nghĩ..."
    
    S->>S: Gửi toàn bộ Audio Buffer lên Google STT
    S->>A: Gửi Text lên Gemini để lấy câu trả lời & Cảm xúc
    A-->>S: Trả về JSON (Text, Emotion)
    
    S->>E: Gửi JSON Cảm xúc {"action": "CHAT_RESPONSE", "emotion": ...}
    Note over E: OLED thay đổi khuôn mặt dựa theo Cảm xúc
    Note over E: Chuyển State: SPEAKING (Tạm dừng VAD)
    
    S->>S: Gửi Text lên Edge-TTS
    S->>S: FFmpeg convert mp3 sang luồng PCM
    
    loop Đang phát âm thanh
        S->>E: Gửi các chunk âm thanh PCM về ESP32 qua WebSocket (BIN)
        E->>E: MAX98357 phát âm thanh ra loa
    end
    
    Note over E: Kết thúc chu kỳ (Không còn Audio). Mở lại VAD.
```

## 3. Lý do sử dụng WebSocket Streaming thay vì HTTP REST

- **Độ trễ thấp:** Dữ liệu âm thanh được bơm (stream) trực tiếp thành các cục (chunk) nhỏ `1024 bytes` mà không cần đợi đóng gói thành một file khổng lồ, loại bỏ toàn bộ overhead của HTTP Header.
- **Phát lại tức thì (Real-time Playback):** Nhờ việc Server phân rã âm thanh bằng `FFmpeg` thành PCM thô và đẩy qua WebSocket, ESP32 chỉ việc vứt thẳng luồng byte này vào IC MAX98357 mà không cần đủ bộ nhớ RAM để tải về hay giải mã MP3.
- **Tiết kiệm tài nguyên:** ESP32 không phải chạy Web Server hay chờ HTTP Response Blocking, nó có dư thời gian để chạy hoạt ảnh GIF 30 fps trên OLED.
