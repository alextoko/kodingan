#ifndef OTA_MANAGER_H
#define OTA_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

class OTAManager {
public:
    OTAManager();

    void begin();
    void loop();

private:
    bool otaStarted;

    void setupOTA();
};

#endif