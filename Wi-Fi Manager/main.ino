#include <Arduino.h>
#include "WiFiManagerCustom.h"
#include "OTAManager.h"

WiFiManagerCustom wifi;
OTAManager ota;

void setup() {

    Serial.begin(115200);

    wifi.begin("ESP32-IoT");
    ota.begin();

}

void loop() {

    wifi.loop(); // library wifi
    ota.loop();  // library ota

    // Program utama ESP32 //
}