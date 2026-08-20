#include "OTAManager.h"

static const char* OTA_HOSTNAME = "ESP32-Awwoak";
static const char* OTA_PASSWORD = "123";

OTAManager::OTAManager()
{
    otaStarted = false;

    lastWiFiState = false;
}

void OTAManager::begin()
{
    Serial.println();
    Serial.println("==============================");
    Serial.println("[OTA] OTA MANAGER");
    Serial.println("==============================");

    if (WiFi.status() == WL_CONNECTED)
    {
        lastWiFiState = true;

        setupOTA();
    }
    else
    {
        lastWiFiState = false;

        Serial.println("[OTA] WiFi belum terhubung.");
        Serial.println("[OTA] OTA menunggu koneksi WiFi.");
    }
}

void OTAManager::loop()
{
    bool wifiConnected =
        (WiFi.status() == WL_CONNECTED);

    if (wifiConnected && !lastWiFiState)
    {
        Serial.println();
        Serial.println("[OTA] WiFi kembali terhubung.");

        lastWiFiState = true;

        setupOTA();
    }

    if (!wifiConnected && lastWiFiState)
    {
        Serial.println();
        Serial.println("[OTA] WiFi terputus.");

        lastWiFiState = false;

        stopOTA();
    }

    if (wifiConnected && otaStarted)
    {
        ArduinoOTA.handle();
    }
}

void OTAManager::setupOTA()
{
    if (otaStarted)
    {
        return;
    }

    ArduinoOTA.setHostname(
        OTA_HOSTNAME
    );

    if (
        OTA_PASSWORD != nullptr &&
        OTA_PASSWORD[0] != '\0'
    )
    {
        ArduinoOTA.setPassword(
            OTA_PASSWORD
        );
    }

    ArduinoOTA.onStart([]()
    {
        Serial.println();
        Serial.println("==============================");
        Serial.println("[OTA] UPDATE DIMULAI");
        Serial.println("==============================");
    });

    ArduinoOTA.onEnd([]()
    {
        Serial.println();
        Serial.println();
        Serial.println("==============================");
        Serial.println("[OTA] UPDATE SELESAI");
        Serial.println("==============================");
    });

    ArduinoOTA.onProgress(
        [](unsigned int progress,
           unsigned int total)
        {
            if (total == 0)
            {
                return;
            }

            unsigned int percent =
                (progress * 100) / total;

            Serial.printf(
                "[OTA] Progress: %u%%\r",
                percent
            );
        }
    );

    ArduinoOTA.onError(
        [](ota_error_t error)
        {
            Serial.printf(
                "\n[OTA] Error[%u]: ",
                error
            );

            switch (error)
            {
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
        }
    );

    ArduinoOTA.begin();

    otaStarted = true;

    Serial.println();
    Serial.println("==============================");
    Serial.println("[OTA] OTA AKTIF");
    Serial.println("==============================");

    Serial.print(
        "[OTA] Hostname : "
    );

    Serial.println(
        OTA_HOSTNAME
    );

    Serial.print(
        "[OTA] IP       : "
    );

    Serial.println(
        WiFi.localIP()
    );

    Serial.print(
        "[OTA] Password : "
    );

    if (
        OTA_PASSWORD != nullptr &&
        OTA_PASSWORD[0] != '\0'
    )
    {
        Serial.println("AKTIF");
    }
    else
    {
        Serial.println("TIDAK ADA");
    }

    Serial.println(
        "=============================="
    );
}

void OTAManager::stopOTA()
{
    if (!otaStarted)
    {
        return;
    }

    ArduinoOTA.end();

    otaStarted = false;

    Serial.println(
        "[OTA] Layanan OTA dihentikan."
    );
}

bool OTAManager::active()
{
    return otaStarted;
}
