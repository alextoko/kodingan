#include "OTAManager.h"

static const char* OTA_HOSTNAME = "sistem-iot";
static const char* OTA_PASSWORD = "085763";

OTAManager::OTAManager()
{
    otaStarted = false;
    lastWiFiState = false;
}

void OTAManager::begin()
{
    const bool wifiConnected = (WiFi.status() == WL_CONNECTED);

    lastWiFiState = wifiConnected;

    if (wifiConnected)
    {
        setupOTA();
    }
}

void OTAManager::loop()
{
    const bool wifiConnected = (WiFi.status() == WL_CONNECTED);

    // WiFi has just connected.
    if (wifiConnected && !lastWiFiState)
    {
        lastWiFiState = true;
        setupOTA();
    }

    // WiFi has just disconnected.
    if (!wifiConnected && lastWiFiState)
    {
        lastWiFiState = false;
        stopOTA();
    }

    // Process OTA only while WiFi is actually connected
    // and OTA has been initialized.
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

    ArduinoOTA.setHostname(OTA_HOSTNAME);
    ArduinoOTA.setPassword(OTA_PASSWORD);

    ArduinoOTA.begin();
    otaStarted = true;
}

void OTAManager::stopOTA()
{
    if (!otaStarted)
    {
        return;
    }

    ArduinoOTA.end();
    otaStarted = false;
}

bool OTAManager::active()
{
    return otaStarted && (WiFi.status() == WL_CONNECTED);
}
