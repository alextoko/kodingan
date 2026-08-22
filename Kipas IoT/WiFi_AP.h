#ifndef WIFI_AP_H
#define WIFI_AP_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

/* =====================================================
   KONFIGURASI
   ===================================================== */

#define LED_WIFI 2

#define WIFI_AP_NAME "Kipas_IoT"


/* =====================================================
   WIFI MANAGER
   Dibuat global supaya tetap tersedia
   ===================================================== */

static WiFiManager wm;


/* =====================================================
   STATUS WIFI
   ===================================================== */

static bool wifiInitialized = false;


/* =====================================================
   TIMER
   ===================================================== */

static uint32_t wifiReconnectTimer = 0;

static const uint32_t WIFI_RECONNECT_INTERVAL = 10000;


/* =====================================================
   INIT WIFI
   ===================================================== */

inline void initWiFi() {

  /* ===================================================
     LED WIFI
     =================================================== */

  pinMode(
    LED_WIFI,
    OUTPUT
  );

  digitalWrite(
    LED_WIFI,
    LOW
  );


  Serial.println();
  Serial.println("[WIFI] Initializing WiFi...");


  /* ===================================================
     MODE WIFI
     =================================================== */

  WiFi.mode(WIFI_STA);


  /* ===================================================
     AUTO CONNECT
     
     ESP32 akan:
     1. Menggunakan WiFi yang sudah tersimpan
     2. Jika belum ada, membuat AP:
        Kipas_IoT
     =================================================== */

  bool connected =
    wm.autoConnect(WIFI_AP_NAME);


  /* ===================================================
     JIKA BERHASIL
     =================================================== */

  if (connected) {

    wifiInitialized = true;

    digitalWrite(
      LED_WIFI,
      HIGH
    );

    Serial.println(
      "[WIFI] Connected"
    );

    Serial.print(
      "[WIFI] IP Address: "
    );

    Serial.println(
      WiFi.localIP()
    );

    return;
  }


  /* ===================================================
     JIKA GAGAL
     
     JANGAN RESTART ESP32
     =================================================== */

  wifiInitialized = false;

  digitalWrite(
    LED_WIFI,
    LOW
  );

  Serial.println(
    "[WIFI] Connection failed"
  );

  Serial.println(
    "[WIFI] ESP32 will continue running"
  );
}


/* =====================================================
   HANDLE WIFI RECONNECT
   ===================================================== */

inline void handleWiFiReconnect() {

  /* ===================================================
     JIKA SUDAH TERHUBUNG
     =================================================== */

  if (WiFi.status() == WL_CONNECTED) {

    wifiInitialized = true;

    return;
  }


  /* ===================================================
     WIFI TERPUTUS
     =================================================== */

  wifiInitialized = false;


  /* ===================================================
     RECONNECT SETIAP 10 DETIK
     =================================================== */

  if (
    millis() - wifiReconnectTimer >=
    WIFI_RECONNECT_INTERVAL
  ) {

    wifiReconnectTimer = millis();


    Serial.println(
      "[WIFI] Attempting reconnect..."
    );


    /* =================================================
       Coba reconnect menggunakan konfigurasi
       WiFi yang sudah tersimpan
       ================================================= */

    WiFi.reconnect();
  }
}


/* =====================================================
   WIFI STATUS LED
   ===================================================== */

inline void wifiStatusLED() {

  static uint32_t t = 0;


  /* ===================================================
     WIFI TERHUBUNG
     LED MENYALA TERUS
     =================================================== */

  if (
    WiFi.status() == WL_CONNECTED
  ) {

    digitalWrite(
      LED_WIFI,
      HIGH
    );

    return;
  }


  /* ===================================================
     WIFI TERPUTUS
     LED BERKEDIP
     =================================================== */

  if (
    millis() - t >= 300
  ) {

    digitalWrite(
      LED_WIFI,
      !digitalRead(LED_WIFI)
    );

    t = millis();
  }
}


/* =====================================================
   STATUS WIFI
   ===================================================== */

inline bool isWiFiConnected() {

  return (
    WiFi.status() == WL_CONNECTED
  );
}


#endif