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

static bool otaReady = false;


/* =====================================================
   INIT OTA
   Dipanggil saat WiFi pertama kali terhubung
   atau setelah WiFi reconnect
   ===================================================== */

inline void initOTA() {

  // Jangan init OTA jika WiFi belum terhubung
  if (WiFi.status() != WL_CONNECTED) {
    otaReady = false;
    return;
  }


  /* ===================================================
     Jika OTA sudah aktif, tidak perlu init ulang
     =================================================== */

  if (otaReady) {
    return;
  }


  Serial.println("[OTA] Initializing OTA...");


  /* ===================================================
     HOSTNAME
     =================================================== */

  ArduinoOTA.setHostname(
    OTA_HOSTNAME
  );


  /* ===================================================
     OTA START
     =================================================== */

  ArduinoOTA.onStart([]() {

    Serial.println(
      "[OTA] Update started"
    );

  });


  /* ===================================================
     OTA END
     =================================================== */

  ArduinoOTA.onEnd([]() {

    Serial.println(
      "\n[OTA] Update finished"
    );

  });


  /* ===================================================
     OTA PROGRESS
     =================================================== */

  ArduinoOTA.onProgress(
    [](unsigned int progress,
       unsigned int total) {

      unsigned int percent =
        (progress * 100U) / total;

      Serial.print(
        "\r[OTA] Progress: "
      );

      Serial.print(
        percent
      );

      Serial.print("%");
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

      Serial.print("]: ");


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


  /* ===================================================
     START OTA
     =================================================== */

  ArduinoOTA.begin();

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
}


/* =====================================================
   HANDLE OTA
   Harus dipanggil terus di loop()
   ===================================================== */

inline void handleOTA() {

  /* ===================================================
     Jika WiFi terputus
     =================================================== */

  if (WiFi.status() != WL_CONNECTED) {

    otaReady = false;

    return;
  }


  /* ===================================================
     Jika WiFi sudah tersambung tetapi OTA belum siap
     
     Ini penting untuk kasus:
     WiFi OFF → WiFi ON
     =================================================== */

  if (!otaReady) {

    initOTA();

    return;
  }


  /* ===================================================
     OTA HANDLE
     =================================================== */

  ArduinoOTA.handle();
}


/* =====================================================
   CEK STATUS OTA
   ===================================================== */

inline bool isOTAReady() {

  return otaReady;
}


#endif