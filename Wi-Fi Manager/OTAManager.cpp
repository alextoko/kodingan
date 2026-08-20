#include "OTAManager.h"

// ======================================================
// KONFIGURASI OTA
// ======================================================

static const char* OTA_HOSTNAME = "MONITORING";
static const char* OTA_PASSWORD = "123";


// ======================================================
// CONSTRUCTOR
// ======================================================

OTAManager::OTAManager() {

    otaStarted = false;
}


// ======================================================
// BEGIN OTA
// ======================================================

void OTAManager::begin() {

    // Pastikan WiFi sudah terhubung
    if (WiFi.status() != WL_CONNECTED) {

        Serial.println();
        Serial.println("[OTA] WiFi belum terhubung.");
        Serial.println("[OTA] Menunggu WiFi...");
        
        return;
    }

    setupOTA();
}


// ======================================================
// SETUP OTA
// ======================================================

void OTAManager::setupOTA() {

    // Jangan inisialisasi dua kali
    if (otaStarted) {
        return;
    }


    // ==================================================
    // HOSTNAME
    // ==================================================

    ArduinoOTA.setHostname(OTA_HOSTNAME);


    // ==================================================
    // PASSWORD
    // ==================================================

    if (OTA_PASSWORD != nullptr && OTA_PASSWORD[0] != '\0') {

        ArduinoOTA.setPassword(OTA_PASSWORD);

    }


    // ==================================================
    // OTA START
    // ==================================================

    ArduinoOTA.onStart([]() {

        Serial.println();
        Serial.println("==============================");
        Serial.println("[OTA] UPDATE DIMULAI");
        Serial.println("==============================");

    });


    // ==================================================
    // OTA END
    // ==================================================

    ArduinoOTA.onEnd([]() {

        Serial.println();
        Serial.println();
        Serial.println("==============================");
        Serial.println("[OTA] UPDATE SELESAI");
        Serial.println("==============================");

    });


    // ==================================================
    // OTA PROGRESS
    // ==================================================

    ArduinoOTA.onProgress([](
        unsigned int progress,
        unsigned int total
    ) {

        if (total == 0) {
            return;
        }

        unsigned int percent =
            (progress * 100) / total;

        Serial.printf(
            "[OTA] Progress: %u%%\r",
            percent
        );

    });


    // ==================================================
    // OTA ERROR
    // ==================================================

    ArduinoOTA.onError([](ota_error_t error) {

        Serial.printf(
            "\n[OTA] Error[%u]: ",
            error
        );


        switch (error) {

            case OTA_AUTH_ERROR:

                Serial.println(
                    "Authentication Failed"
                );

                break;


            case OTA_BEGIN_ERROR:

                Serial.println(
                    "Begin Failed"
                );

                break;


            case OTA_CONNECT_ERROR:

                Serial.println(
                    "Connect Failed"
                );

                break;


            case OTA_RECEIVE_ERROR:

                Serial.println(
                    "Receive Failed"
                );

                break;


            case OTA_END_ERROR:

                Serial.println(
                    "End Failed"
                );

                break;


            default:

                Serial.println(
                    "Unknown Error"
                );

                break;
        }

    });


    // ==================================================
    // MULAI OTA
    // ==================================================

    ArduinoOTA.begin();

    otaStarted = true;


    // ==================================================
    // INFORMASI OTA
    // ==================================================

    Serial.println();
    Serial.println("==============================");
    Serial.println("[OTA] OTA AKTIF");
    Serial.println("==============================");

    Serial.print("[OTA] Hostname : ");
    Serial.println(OTA_HOSTNAME);

    Serial.print("[OTA] IP       : ");
    Serial.println(WiFi.localIP());

    Serial.print("[OTA] Password : ");

    if (OTA_PASSWORD != nullptr &&
        OTA_PASSWORD[0] != '\0') {

        Serial.println("AKTIF");

    } else {

        Serial.println("TIDAK ADA");

    }

    Serial.println("==============================");
}


// ======================================================
// LOOP OTA
// ======================================================

void OTAManager::loop() {

    // ==================================================
    // WIFI BELUM TERHUBUNG
    // ==================================================

    if (WiFi.status() != WL_CONNECTED) {

        // Jika sebelumnya OTA aktif,
        // hentikan layanan OTA.
        if (otaStarted) {

            ArduinoOTA.end();

            otaStarted = false;

            Serial.println();
            Serial.println("[OTA] WiFi terputus.");
            Serial.println("[OTA] Layanan OTA dihentikan.");
        }

        return;
    }


    // ==================================================
    // WIFI SUDAH TERHUBUNG,
    // TAPI OTA BELUM AKTIF
    // ==================================================

    if (!otaStarted) {

        Serial.println();
        Serial.println("[OTA] WiFi kembali terhubung.");
        Serial.println("[OTA] Mengaktifkan kembali OTA...");

        setupOTA();

        return;
    }


    // ==================================================
    // OTA AKTIF
    // ==================================================

    ArduinoOTA.handle();
}