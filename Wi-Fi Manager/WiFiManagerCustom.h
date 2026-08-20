#ifndef WIFI_MANAGER_CUSTOM_H
#define WIFI_MANAGER_CUSTOM_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

class WiFiManagerCustom {
public:
    WiFiManagerCustom();

    void begin(const char* apName = "ESP32-IoT");
    void loop();

    bool connected();
    String getIP();
    String getSSID();
    int getRSSI();

private:
    WiFiManager wm;

    const char* apName;

    unsigned long lastReconnectAttempt;
    unsigned long lastLedBlink;

    static const unsigned long RECONNECT_INTERVAL = 10000;
    static const unsigned long LED_BLINK_INTERVAL = 500;

    static const int WIFI_LED_PIN = 2;

    bool ledState;

    void reconnect();
    void updateLED();
};

#endif