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

  /* ===================================================
     WIFI HARUS TERHUBUNG
     =================================================== */

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println(
      "[NETWORK] WiFi not connected"
    );

    return;
  }


  Serial.println();
  Serial.println(
    "[NETWORK] WiFi connected"
  );

  Serial.print(
    "[NETWORK] IP Address: "
  );

  Serial.println(
    WiFi.localIP()
  );


  /* ===================================================
     NTP
     =================================================== */

  initNTP();


  /* ===================================================
     OTA
     =================================================== */

  bool otaOK =
    initOTA();


  if (otaOK) {

    Serial.println(
      "[NETWORK] OTA ready"
    );

  } else {

    Serial.println(
      "[NETWORK] OTA not ready"
    );
  }


  /* ===================================================
     SINRIC
     =================================================== */

  bool sinricOK =
    initSinric();


  if (sinricOK) {

    sinricReady = true;

    Serial.println(
      "[NETWORK] Sinric ready"
    );

  } else {

    sinricReady = false;

    Serial.println(
      "[NETWORK] Sinric not ready"
    );
  }


  Serial.println(
    "[NETWORK] Network services initialized"
  );
}


/* =====================================================
   SETUP
   ===================================================== */

void setup() {

  Serial.begin(
    115200
  );

  delay(100);


  Serial.println();
  Serial.println(
    "=============================="
  );

  Serial.println(
    "       KIPAS IoT ESP32"
  );

  Serial.println(
    "=============================="
  );


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

  if (
    WiFi.status() == WL_CONNECTED
  ) {

    lastWiFiConnected = true;

    initNetworkServices();

  }

  else {

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
     
     Ditangani oleh WiFi_AP.h
     =================================================== */

  handleWiFiReconnect();


  /* ===================================================
     BACA STATUS WIFI TERKINI
     =================================================== */

  bool currentWiFiConnected =
    (
      WiFi.status() == WL_CONNECTED
    );


  /* ===================================================
     WIFI TERPUTUS
     =================================================== */

  if (
    lastWiFiConnected &&
    !currentWiFiConnected
  ) {

    Serial.println();
    Serial.println(
      "[WIFI] Connection lost"
    );


    /* ================================================
       Sinric tidak dapat digunakan sementara
       ================================================ */

    sinricReady = false;
  }


  /* ===================================================
     WIFI TERSAMBUNG KEMBALI
     =================================================== */

  if (
    !lastWiFiConnected &&
    currentWiFiConnected
  ) {

    Serial.println();
    Serial.println(
      "[WIFI] Connection restored"
    );


    /* ================================================
       Inisialisasi ulang service jaringan

       NTP  → konfigurasi ulang waktu
       OTA  → aktif kembali
       Sinric → tandai sync pending
       ================================================ */

    initNetworkServices();
  }


  /* ===================================================
     SIMPAN STATUS WIFI
     =================================================== */

  lastWiFiConnected =
    currentWiFiConnected;


  /* ===================================================
     RELAY EXCLUSIVE TASK
     
     PENTING:
     
     Jangan masukkan ke dalam:
     
         if (sinricReady)
     
     Karena pending ON/OFF relay harus tetap diproses
     walaupun Sinric sedang offline.
     =================================================== */

  handleExclusiveTask(
    switchID
  );


  /* ===================================================
     SINRIC
     =================================================== */

  if (sinricReady) {

    handleSinric();
  }


  /* ===================================================
     OTA
     
     handleOTA() tetap dipanggil terus.
     
     Jika WiFi putus:
       → langsung return
     
     Jika WiFi kembali:
       → initOTA()
     
     Jika OTA aktif:
       → ArduinoOTA.handle()
     =================================================== */

  handleOTA();


  /* ===================================================
     SCHEDULER
     
     Scheduler tidak bergantung pada Sinric.
     =================================================== */

  handleAutoOffAtMidnight(
    switchID
  );
}