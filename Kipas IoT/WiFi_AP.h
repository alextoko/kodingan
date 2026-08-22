#ifndef WIFI_AP_H
#define WIFI_AP_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

/* ===== KONFIG ===== */
#define WIFI_AP_NAME          "Kipas_IoT"
#define WIFI_CONNECT_TIMEOUT  10          // detik (WiFiManager)
#define WIFI_RETRY_INTERVAL   10000       // ms
#define LED_STATUS            2

/* ===== STATE ===== */
static unsigned long lastWifiCheck = 0;
static bool wifiConnectedOnce = false;

/* ===== INIT PIN ===== */
inline void wifiSetupPins() {
  pinMode(LED_STATUS, OUTPUT);
  digitalWrite(LED_STATUS, LOW);
}

/* ===== CONNECT / PROVISION ===== */
inline bool initWiFiManager() {
  wifiSetupPins();

  WiFiManager wm;
  wm.setConnectTimeout(WIFI_CONNECT_TIMEOUT);

  Serial.println("[WiFi] Checking saved WiFi / Starting portal if needed...");
  bool ok = wm.autoConnect(WIFI_AP_NAME); // BLOCKING hanya saat provisioning

  if (ok) {
    Serial.println("[WiFi] Connected");
    Serial.print("[WiFi] IP: ");
    Serial.println(WiFi.localIP());

    digitalWrite(LED_STATUS, HIGH); // LED ON = WiFi connected
    wifiConnectedOnce = true;
  } else {
    // kondisi ini jarang terjadi (autoConnect biasanya blocking)
    Serial.println("[WiFi] Portal active");
  }

  return ok;
}

/* ===== MONITOR STATUS (NON-BLOCKING) ===== */
inline void wifiHandleStatus() {

  if (WiFi.status() == WL_CONNECTED) {
    // WiFi CONNECTED → LED NYALA
    digitalWrite(LED_STATUS, HIGH);
    return;
  }

  // WiFi TIDAK CONNECTED → LED KEDIP
  digitalWrite(LED_STATUS, (millis() % 1000) < 500);

  // Auto reconnect berkala (jika sebelumnya pernah connect)
  if (wifiConnectedOnce &&
      millis() - lastWifiCheck >= WIFI_RETRY_INTERVAL) {

    Serial.println("[WiFi] Reconnecting...");
    WiFi.reconnect();
    lastWifiCheck = millis();
  }
}

/* ===== HELPER ===== */
inline bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

#endif
