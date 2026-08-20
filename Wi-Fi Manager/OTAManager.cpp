#include "OTAManager.h"

static const char* OTA_HOSTNAME = "heheh";
static const char* OTA_PASSWORD = "123";

OTAManager::OTAManager()
{
    otaStarted = false;
    lastWiFiState = false;
}

void OTAManager::begin()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        lastWiFiState = true;
        setupOTA();
    }
}

void OTAManager::loop()
{
    bool wifiConnected =
        (WiFi.status() == WL_CONNECTED);

    if (wifiConnected && !lastWiFiState)
    {
        lastWiFiState = true;
        setupOTA();
    }

    if (!wifiConnected && lastWiFiState)
    {
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
        return;

    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.begin();

    otaStarted = true;
}

void OTAManager::stopOTA()
{
    if (!otaStarted)
        return;

    ArduinoOTA.end();

    otaStarted = false;
}

bool OTAManager::active()
{
    return otaStarted;
}