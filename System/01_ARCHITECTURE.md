# Architecture

```
main
 │
 ▼
Application
 │
 ├──────── ILogger
 │
 ├──────── IWiFiManager
 │
 ├──────── IHttpClient
 │
 ├──────── IDisplay
 │
 ├──────── IButton
 │
 └──────── ISensor
```

Application chỉ điều phối.

Không được tạo object.

Không new.

Không delete.

Không biết implementation.

---

## Composition Root

main.cpp

Là nơi duy nhất tạo object.

Ví dụ

```cpp
Logger logger;

WiFiManager wifi(logger);

DisplayManager display(logger);

HttpClient http(logger, wifi);

Application app(
    logger,
    wifi,
    display,
    http
);
```

Không module nào được tự new module khác.

---

## Dependency Injection

Tất cả dependency phải truyền qua constructor.

Ví dụ

```cpp
class WiFiManager
{
public:

    explicit WiFiManager(
        ILogger& logger
    );
};
```

Không được

```cpp
Logger logger;
```

ở bên trong.

---

## Interface

Mọi module đều phải có Interface.

Ví dụ

ILogger

IWiFiManager

IDisplay

IHttpClient

IButton

ISensor

Application chỉ sử dụng Interface.

Không sử dụng implementation.

---

## Lifecycle

Mọi module phải có

```cpp
begin()

loop()
```

Nếu cần.

Không module nào tự chạy.

Application điều phối toàn bộ.

---

## Logging

Không được gọi

Serial.println()

ở bất kỳ module nào.

Chỉ Logger được phép dùng Serial.

Mọi module log thông qua

ILogger.