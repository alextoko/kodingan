#include "OTAManager.h"

OTAManager::OTAManager() {
    otaStarted = false;
}

void OTAManager::begin() {

    // Pastikan WiFi sudah terhubung
    if (WiFi.status() != WL_CONNECTED) {

        Serial.println();
        Serial.println("[OTA] WiFi belum terhubung.");
        Serial.println("[OTA] OTA belum dimulai.");

        return;
    }

    setupOTA();
}

void OTAManager::setupOTA() {

    if (otaStarted) {
        return;
    }

    // Nama ESP32 yang muncul di Arduino IDE
    ArduinoOTA.setHostname("ESP32-MONITORING");

    // ==============================
    // OTA START
    // ==============================

    ArduinoOTA.onStart([]() {

        Serial.println();
        Serial.println("==============================");
        Serial.println("[OTA] UPDATE DIMULAI");
        Serial.println("==============================");

    });

    // ==============================
    // OTA END
    // ==============================

    ArduinoOTA.onEnd([]() {

        Serial.println();
        Serial.println();
        Serial.println("==============================");
        Serial.println("[OTA] UPDATE SELESAI");
        Serial.println("==============================");

    });

    // ==============================
    // OTA PROGRESS
    // ==============================

    ArduinoOTA.onProgress([](
        unsigned int progress,
        unsigned int total
    ) {

        unsigned int percent =
            (progress * 100) / total;

        Serial.printf(
            "[OTA] Progress: %u%%\r",
            percent
        );
    });

    // ==============================
    // OTA ERROR
    // ==============================

    ArduinoOTA.onError([](ota_error_t error) {

        Serial.printf(
            "\n[OTA] Error[%u]: ",
            error
        );

        switch (error) {

            case OTA_AUTH_ERROR:
                Serial.println("Authentication Failed");
                break;

            case OTA_BEGIN_ERROR:
                Serial.println("Begin Failed");
                break;

            case OTA_CONNECT_ERROR:
                Serial.println("Connect Failed");
                break;

            case OTA_RECEIVE_ERROR:
                Serial.println("Receive Failed");
                break;

            case OTA_END_ERROR:
                Serial.println("End Failed");
                break;

            default:
                Serial.println("Unknown Error");
                break;
        }
    });

    // Mulai OTA
    ArduinoOTA.begin();

    otaStarted = true;

    Serial.println();
    Serial.println("==============================");
    Serial.println("[OTA] OTA AKTIF");
    Serial.println("==============================");

    Serial.print("[OTA] Hostname : ");
    Serial.println("ESP32-MONITORING");

    Serial.print("[OTA] IP       : ");
    Serial.println(WiFi.localIP());

    Serial.println("==============================");
}

void OTAManager::loop() {

    // Jika OTA belum aktif
    if (!otaStarted) {

        // Coba aktifkan kembali jika WiFi sudah terhubung
        if (WiFi.status() == WL_CONNECTED) {
            setupOTA();
        }

        return;
    }

    // Jika WiFi terputus
    if (WiFi.status() != WL_CONNECTED) {
        return;
    }

    // Menangani proses OTA
    ArduinoOTA.handle();
}