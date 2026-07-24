# Development Rules

## Naming

Class

```cpp
WiFiManager
```

Interface

```cpp
IWiFiManager
```

Enum

```cpp
enum class WiFiState
```

Method

camelCase

```cpp
connect()

disconnect()

isConnected()
```

Constant

```cpp
constexpr
```

Namespace

```cpp
namespace AppConfig
```

---

## Constructor

Dependency truyền qua constructor.

Ví dụ

```cpp
WiFiManager(
    ILogger&
);
```

Không new object.

---

## Memory

Không malloc.

Không free.

Không new.

Không delete.

Không cấp phát động nếu không cần.

---

## String

Không dùng

```cpp
String
```

trong API framework.

Có thể dùng

```cpp
const char*

std::string
```

nếu PlatformIO.

---

## Logger

Logger là module duy nhất được phép gọi

```cpp
Serial.print
```

Các module khác tuyệt đối không.

---

## Error Handling

Không return int.

Ưu tiên

```cpp
enum class Result
```

Ví dụ

```cpp
enum class HttpResult
{
    Success,

    Timeout,

    ConnectionFailed,

    InvalidResponse
};
```

---

## Future Modules

Framework phải có khả năng mở rộng:

- MQTT
- OTA
- Firebase
- BLE
- Camera
- SD Card
- Preferences
- JSON
- WebSocket

mà không cần sửa Application.

---

## Testing

Mỗi module phải có thể test độc lập.

Không phụ thuộc module khác.

Có thể thay bằng Mock thông qua Interface.

Ví dụ

```cpp
MockWiFiManager
```

để test Application.