#ifndef OTA_H
#define OTA_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

inline void initOTA() {
  if (WiFi.status() != WL_CONNECTED) return;
  ArduinoOTA.setHostname("Kipas_IoT");
  ArduinoOTA.begin();
}

inline void handleOTA() {
  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.handle();
  }
}

#endif
