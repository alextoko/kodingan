#ifndef OTA_H
#define OTA_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

/* ===== KONFIG OTA ===== */
#define OTA_HOSTNAME "Kipas_IoT"
// gunakan hash untuk keamanan (contoh hash md5 dari "123456")
#define OTA_PASSWORD_HASH "e10adc3949ba59abbe56e057f20f883e"

static bool otaReady = false;

inline void setupOTA() {
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPasswordHash(OTA_PASSWORD_HASH);

  ArduinoOTA
    .onStart([]() {
      Serial.println("[OTA] Start");
    })
    .onEnd([]() {
      Serial.println("[OTA] End");
      delay(300);
      ESP.restart();
    })
    .onProgress([](unsigned int p, unsigned int t) {
      static uint32_t last = 0;
      if (millis() - last > 1000) {
        last = millis();
        Serial.printf("[OTA] %u%%\n", (p * 100) / t);
      }
    })
    .onError([](ota_error_t e) {
      Serial.printf("[OTA] Error %u\n", e);
    });

  ArduinoOTA.begin();
  otaReady = true;
  Serial.println("[OTA] Ready");
}

inline void handleOTA() {
  if (otaReady && WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.handle();
  }
}

inline bool isOTAReady() {
  return otaReady;
}

#endif
