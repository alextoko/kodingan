#include "WIFI_AP.h"
#include "OTA.h"
#include "Time_NTP.h"   

unsigned long lastPrint = 0;  // ← WAJIB ADA

void setup() {
  Serial.begin(115200);
  delay(100);

  // Init WiFi
  initWiFiManager();

  // Init OTA & NTP hanya jika WiFi terhubung
  if (WiFi.status() == WL_CONNECTED) {
    setupOTA();
    initNTPTime();   // ← SEKARANG DIKENAL
  }
}

void loop() {
  wifiHandleStatus();
  handleOTA();

  // Tampilkan jam tiap 1 detik
  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();

    Serial.print("Jam     : ");
    Serial.print(getTimeString());
    Serial.print(" | Tanggal : ");
    Serial.println(getDateString());
  }
}
