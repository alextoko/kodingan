#ifndef WIFI_AP_H
#define WIFI_AP_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

#define LED_WIFI 2

inline void initWiFi() {
  pinMode(LED_WIFI, OUTPUT);
  digitalWrite(LED_WIFI, LOW);

  WiFiManager wm;
  if (!wm.autoConnect("Kipas_IoT")) {
    ESP.restart();
  }
  digitalWrite(LED_WIFI, HIGH);
}

inline void wifiStatusLED() {
  static uint32_t t = 0;
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_WIFI, HIGH);
  } else if (millis() - t > 300) {
    digitalWrite(LED_WIFI, !digitalRead(LED_WIFI));
    t = millis();
  }
}

#endif
