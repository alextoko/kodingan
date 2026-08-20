#include <Arduino.h>
#include "WiFiManagerCustom.h"
#include "OTAManager.h"

WiFiManagerCustom wifi;
OTAManager ota;

void setup() {

    Serial.begin(115200);

    wifi.begin("ESP32-MONITORING");
    ota.begin();
}

void loop() {

    wifi.loop();
    ota.loop();

    // Program utama ESP32
}