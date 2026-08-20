#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

class OTAManager
{
public:

    OTAManager();

    void begin();

    void loop();

    bool active();

private:

    bool otaStarted;

    bool lastWiFiState;

    void setupOTA();

    void stopOTA();
};

#endif