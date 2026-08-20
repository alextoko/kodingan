#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

class WiFiManager
{
public:

    WiFiManager(
        const char* ssid,
        const char* password,
        uint8_t ledPin = 2
    );

    void begin();

    void update();

    bool isConnected();

    String getIP();

private:

    const char* _ssid;
    const char* _password;

    uint8_t _ledPin;

    bool _wifiConnected;

    unsigned long _lastReconnect;
    unsigned long _lastBlink;

    bool _ledState;
};

#endif