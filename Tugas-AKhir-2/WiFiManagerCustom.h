#ifndef WIFI_MANAGER_CUSTOM_H
#define WIFI_MANAGER_CUSTOM_H
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
class WiFiManagerCustom {public: WiFiManagerCustom(); void begin(const char* apName="ESP32-WIFI-MANAGER"); void loop(); bool connected(); String getIP(); String getSSID(); int getRSSI(); private: WiFiManager wm; const char* apName; unsigned long lastReconnectAttempt,lastLedBlink,reconnectStart; static const unsigned long STARTUP_CONNECT_TIMEOUT=10000,RECONNECT_INTERVAL=10000,RECONNECT_TIMEOUT=8000,LED_BLINK_INTERVAL=500,PORTAL_TIMEOUT=60000; static const uint8_t MAX_RECONNECT_ATTEMPTS=5; static const int WIFI_LED_PIN=2; bool ledState,wasConnected,reconnectInProgress,portalActive; uint8_t reconnectAttempts; void reconnect(); void updateLED(); void startConfigPortal(); bool tryStoredWiFi(unsigned long timeoutMs);};
#endif
