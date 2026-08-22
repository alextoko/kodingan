#include "WiFi_AP.h"
#include "OTA.h"
#include "Time_NTP.h"
#include "relay_ctrl.h"
#include "sinric_ctrl.h"
#include "scheduler.h"


/* =====================================================
   STATUS SINRIC
   ===================================================== */

bool sinricReady = false;


/* =====================================================
   STATUS WIFI SEBELUMNYA
   ===================================================== */

bool lastWiFiConnected = false;


/* =====================================================
   INISIALISASI SERVICE NETWORK
   ===================================================== */

void initNetworkServices() {

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  Serial.println();
  Serial.println("[NETWORK] WiFi connected");

  Serial.print("[NETWORK] IP Address: ");
  Serial.println(WiFi.localIP());


  /* ===================================================
     NTP
     =================================================== */

  initNTP();


  /* ===================================================
     OTA
     =================================================== */

  initOTA();


  /* ===================================================
     SINRIC
     =================================================== */

  initSinric();

  sinricReady = true;


  Serial.println(
    "[NETWORK] All network services ready"
  );
}


/* =====================================================
   SETUP
   ===================================================== */

void setup() {

  Serial.begin(115200);

  delay(100);

  Serial.println();
  Serial.println("==============================");
  Serial.println("       KIPAS IoT ESP32");
  Serial.println("==============================");


  /* ===================================================
     RELAY
     =================================================== */

  initRelay();


  /* ===================================================
     WIFI
     =================================================== */

  initWiFi();


  /* ===================================================
     CEK WIFI
     =================================================== */

  if (WiFi.status() == WL_CONNECTED) {

    lastWiFiConnected = true;

    initNetworkServices();

  } else {

    lastWiFiConnected = false;

    Serial.println(
      "[WIFI] Not connected"
    );
  }
}


/* =====================================================
   LOOP
   ===================================================== */

void loop() {

  /* ===================================================
     WIFI LED
     =================================================== */

  wifiStatusLED();


  /* ===================================================
     WIFI RECONNECT
     
     Fungsi berasal dari WiFi_AP.h
     =================================================== */

  handleWiFiReconnect();


  /* ===================================================
     DETEKSI WIFI CONNECT / DISCONNECT
     =================================================== */

  bool currentWiFiConnected =
    (WiFi.status() == WL_CONNECTED);


  /* ===================================================
     WIFI TERPUTUS
     =================================================== */

  if (
    lastWiFiConnected &&
    !currentWiFiConnected
  ) {

    Serial.println(
      "[WIFI] Connection lost"
    );

    sinricReady = false;
  }


  /* ===================================================
     WIFI TERSAMBUNG KEMBALI
     =================================================== */

  if (
    !lastWiFiConnected &&
    currentWiFiConnected
  ) {

    Serial.println(
      "[WIFI] Connection restored"
    );

    initNetworkServices();
  }


  /* ===================================================
     SIMPAN STATUS WIFI
     =================================================== */

  lastWiFiConnected =
    currentWiFiConnected;


  /* ===================================================
     SINRIC
     =================================================== */

  if (sinricReady) {

    handleExclusiveTask(
      switchID
    );

    handleSinric();
  }


  /* ===================================================
     OTA
     =================================================== */

  handleOTA();


  /* ===================================================
     SCHEDULER
     =================================================== */

  handleAutoOffAtMidnight(
    switchID
  );
}