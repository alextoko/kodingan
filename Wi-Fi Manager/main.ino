#include <Arduino.h>
#include "WiFiManagerCustom.h"

WiFiManagerCustom wifi;

void setup() {

    Serial.begin(115200);

    wifi.begin("ESP32-MONITORING");
}

void loop() {

    // WiFi auto reconnect + indikator LED GPIO 2
    wifi.loop();

    // Program utama ESP32
}