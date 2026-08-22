#ifndef OTA_H
#define OTA_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>


/* =====================================================
   KONFIGURASI OTA
   ===================================================== */

static const char* OTA_HOSTNAME = "Kipas_IoT";


/* =====================================================
   STATUS OTA
   ===================================================== */

// Callback OTA sudah didaftarkan?
static bool otaCallbacksRegistered = false;

// ArduinoOTA.begin() sudah pernah dijalankan?
static bool otaStarted = false;

// OTA siap digunakan pada koneksi WiFi saat ini?
static bool otaReady = false;


/* =====================================================
   REGISTER CALLBACK OTA
   HANYA SEKALI SELAMA ESP32 HIDUP
   ===================================================== */

inline void registerOTACallbacks() {

  if (otaCallbacksRegistered) {
    return;
  }


  Serial.println(
    "[OTA] Registering callbacks..."
  );


  /* ===================================================
     OTA START
     =================================================== */

  ArduinoOTA.onStart([]() {

    Serial.println();
    Serial.println(
      "[OTA] Update started"
    );

  });


  /* ===================================================
     OTA END
     =================================================== */

  ArduinoOTA.onEnd([]() {

    Serial.println();
    Serial.println(
      "[OTA] Update finished"
    );

  });


  /* ===================================================
     OTA PROGRESS
     =================================================== */

  ArduinoOTA.onProgress(
    [](unsigned int progress,
       unsigned int total) {

      if (total == 0) {
        return;
      }


      unsigned int percent =
        (progress * 100U) / total;


      Serial.print(
        "\r[OTA] Progress: "
      );

      Serial.print(
        percent
      );

      Serial.print(
        "%"
      );
    }
  );


  /* ===================================================
     OTA ERROR
     =================================================== */

  ArduinoOTA.onError(
    [](ota_error_t error) {

      Serial.print(
        "\n[OTA] Error["
      );

      Serial.print(
        error
      );

      Serial.print(
        "]: "
      );


      if (error == OTA_AUTH_ERROR) {

        Serial.println(
          "Authentication Failed"
        );

      }

      else if (error == OTA_BEGIN_ERROR) {

        Serial.println(
          "Begin Failed"
        );

      }

      else if (error == OTA_CONNECT_ERROR) {

        Serial.println(
          "Connect Failed"
        );

      }

      else if (error == OTA_RECEIVE_ERROR) {

        Serial.println(
          "Receive Failed"
        );

      }

      else if (error == OTA_END_ERROR) {

        Serial.println(
          "End Failed"
        );

      }

      else {

        Serial.println(
          "Unknown Error"
        );
      }
    }
  );


  otaCallbacksRegistered = true;


  Serial.println(
    "[OTA] Callbacks registered"
  );
}


/* =====================================================
   INIT OTA
   Dipanggil saat:
   - boot
   - WiFi reconnect
   ===================================================== */

inline bool initOTA() {

  /* ===================================================
     WIFI HARUS TERHUBUNG
     =================================================== */

  if (WiFi.status() != WL_CONNECTED) {

    otaReady = false;

    return false;
  }


  Serial.println();
  Serial.println(
    "[OTA] Initializing OTA..."
  );


  /* ===================================================
     SET HOSTNAME
     =================================================== */

  ArduinoOTA.setHostname(
    OTA_HOSTNAME
  );


  /* ===================================================
     REGISTER CALLBACK
     
     Hanya sekali.
     =================================================== */

  registerOTACallbacks();


  /* ===================================================
     ArduinoOTA.begin()
     
     Hanya sekali selama ESP32 hidup.
     =================================================== */

  if (!otaStarted) {

    ArduinoOTA.begin();

    otaStarted = true;


    Serial.println(
      "[OTA] ArduinoOTA started"
    );

  }
  else {

    Serial.println(
      "[OTA] ArduinoOTA already started"
    );
  }


  /* ===================================================
     OTA SIAP
     =================================================== */

  otaReady = true;


  Serial.print(
    "[OTA] Ready. Hostname: "
  );

  Serial.println(
    OTA_HOSTNAME
  );


  Serial.print(
    "[OTA] IP Address: "
  );

  Serial.println(
    WiFi.localIP()
  );


  return true;
}


/* =====================================================
   HANDLE OTA
   Harus dipanggil terus dari loop()
   ===================================================== */

inline void handleOTA() {

  /* ===================================================
     WIFI PUTUS
     =================================================== */

  if (WiFi.status() != WL_CONNECTED) {

    otaReady = false;

    return;
  }


  /* ===================================================
     WIFI SUDAH KEMBALI
     TAPI OTA BELUM SIAP
     =================================================== */

  if (!otaReady) {

    initOTA();

    return;
  }


  /* ===================================================
     OTA BELUM PERNAH DIMULAI
     =================================================== */

  if (!otaStarted) {

    initOTA();

    return;
  }


  /* ===================================================
     PROSES OTA
     =================================================== */

  ArduinoOTA.handle();
}


/* =====================================================
   DIPANGGIL SAAT WIFI RECONNECT
   ===================================================== */

inline void reconnectOTA() {

  if (WiFi.status() != WL_CONNECTED) {

    otaReady = false;

    return;
  }


  Serial.println(
    "[OTA] WiFi restored"
  );


  /*
     Tidak perlu mendaftarkan callback lagi.
     Tidak perlu ArduinoOTA.begin() lagi.

     Cukup tandai OTA siap digunakan kembali.
  */

  otaReady = true;


  Serial.print(
    "[OTA] OTA available at "
  );

  Serial.println(
    WiFi.localIP()
  );
}


/* =====================================================
   STATUS OTA
   ===================================================== */

inline bool isOTAReady() {

  return otaReady;
}


/* =====================================================
   STATUS CALLBACK
   ===================================================== */

inline bool areOTACallbacksRegistered() {

  return otaCallbacksRegistered;
}


/* =====================================================
   STATUS OTA STARTED
   ===================================================== */

inline bool isOTAStarted() {

  return otaStarted;
}


#endif