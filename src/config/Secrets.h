#ifndef SECRETS_H
#define SECRETS_H

#include <Arduino.h>

namespace Secrets
{
    struct WiFiCredential {
        const char* ssid;
        const char* password;
    };

    // Bạn có thể thêm bao nhiêu mạng WiFi vào danh sách này cũng được.
    // ESP32 sẽ tự động dò và kết nối với mạng nào đang phát (và có sóng mạnh nhất).
    constexpr WiFiCredential WIFI_NETWORKS[] = {
        {"KHANH CHAU 2.4G", "khanhchau04"},
        {"Tro Nha", "khanhchau04"},
        {"TEN_WIFI_3", "MAT_KHAU_3"}
    };
    
    constexpr size_t WIFI_NETWORKS_COUNT = sizeof(WIFI_NETWORKS) / sizeof(WIFI_NETWORKS[0]);
}

#endif // SECRETS_H
