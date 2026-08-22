#ifndef SINRIC_CTRL_H
#define SINRIC_CTRL_H

#include <Arduino.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"
#include "relay_ctrl.h"


/* =====================================================
   SINRIC PRO CONFIGURATION
   ===================================================== */

// JANGAN hard-code credential yang sudah terekspos.
// Ganti dengan APP_KEY dan APP_SECRET baru milik Anda.

#define APP_KEY     "d43d4bd8-0241-4997-a46d-e3ffffb0eb5f"
#define APP_SECRET  "5853eab2-8c69-497f-8eaa-fcd9e28d02b8-b97279cc-2f7c-4487-bc10-364eb25f49ac"


/* =====================================================
   SINRIC DEVICE ID
   ===================================================== */

static const char* switchID[3] = {
  "6a8871c829c6be334299415f",
  "6a887202969af7ec248d2b88",
  "6a88721a969af7ec248d2bb7"
};


/* =====================================================
   KONFIGURASI JUMLAH SWITCH
   ===================================================== */

static const uint8_t SINRIC_SWITCH_COUNT = 3;


/* =====================================================
   STATUS SINRIC
   ===================================================== */

static bool sinricInitialized = false;


/* =====================================================
   CALLBACK POWER STATE
   ===================================================== */

inline bool onPower(
  const String &deviceId,
  bool &state
) {

  Serial.println();
  Serial.println("[SINRIC] Power command received");

  Serial.print("[SINRIC] Device ID: ");
  Serial.println(deviceId);

  Serial.print("[SINRIC] Requested state: ");
  Serial.println(
    state ? "ON" : "OFF"
  );


  /* ===================================================
     CARI DEVICE ID
     =================================================== */

  for (
    uint8_t i = 0;
    i < SINRIC_SWITCH_COUNT;
    i++
  ) {

    if (deviceId == switchID[i]) {

      Serial.print(
        "[SINRIC] Relay target: "
      );

      Serial.println(
        i + 1
      );


      /* ================================================
         Kirim perintah ke relay controller

         requestExclusive() akan:
         ON  -> pending 500 ms
         OFF -> langsung OFF + cancel pending ON
         ================================================ */

      requestExclusive(
        i,
        state,
        switchID
      );

      return true;
    }
  }


  /* ===================================================
     DEVICE TIDAK DIKENAL
     =================================================== */

  Serial.println(
    "[SINRIC] Unknown device ID"
  );

  return false;
}


/* =====================================================
   REGISTER CALLBACK
   HANYA SEKALI
   ===================================================== */

inline void registerSinricCallbacks() {

  if (sinricInitialized) {
    return;
  }


  Serial.println(
    "[SINRIC] Registering callbacks..."
  );


  for (
    uint8_t i = 0;
    i < SINRIC_SWITCH_COUNT;
    i++
  ) {

    SinricProSwitch &sw =
      SinricPro[switchID[i]];

    sw.onPowerState(
      onPower
    );
  }


  sinricInitialized = true;


  Serial.println(
    "[SINRIC] Callbacks registered"
  );
}


/* =====================================================
   INIT SINRIC
   Dipanggil saat boot dan WiFi reconnect
   ===================================================== */

inline void initSinric() {

  /* ===================================================
     Jangan init jika WiFi belum terhubung
     =================================================== */

  if (WiFi.status() != WL_CONNECTED) {

    Serial.println(
      "[SINRIC] WiFi not connected"
    );

    return;
  }


  Serial.println();
  Serial.println(
    "[SINRIC] Initializing SinricPro..."
  );


  /* ===================================================
     CALLBACK HANYA SEKALI
     =================================================== */

  registerSinricCallbacks();


  /* ===================================================
     MULAI / HUBUNGKAN SINRIC
     =================================================== */

  SinricPro.begin(
    APP_KEY,
    APP_SECRET
  );


  /* ===================================================
     SINKRONISASI STATE RELAY LOKAL
     
     Relay lokal adalah sumber state sebenarnya.
     =================================================== */

  Serial.println(
    "[SINRIC] Synchronizing relay states..."
  );


  for (
    uint8_t i = 0;
    i < SINRIC_SWITCH_COUNT;
    i++
  ) {

    bool state =
      getRelayState(i);


    SinricProSwitch &sw =
      SinricPro[switchID[i]];


    sw.sendPowerStateEvent(
      state
    );


    Serial.print(
      "[SINRIC] Relay "
    );

    Serial.print(
      i + 1
    );

    Serial.print(
      " -> "
    );

    Serial.println(
      state ? "ON" : "OFF"
    );
  }


  Serial.println(
    "[SINRIC] SinricPro initialized"
  );
}


/* =====================================================
   HANDLE SINRIC
   ===================================================== */

inline void handleSinric() {

  /* ===================================================
     Jangan jalankan Sinric ketika WiFi terputus
     =================================================== */

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }


  /* ===================================================
     Jalankan SinricPro
     =================================================== */

  SinricPro.handle();
}


#endif