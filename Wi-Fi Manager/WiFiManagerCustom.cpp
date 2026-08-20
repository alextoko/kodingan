#include "WiFiManagerCustom.h"

WiFiManagerCustom::WiFiManagerCustom() {

    apName = "ESP32-WIFI-MANAGER";

    lastReconnectAttempt = 0;
    lastLedBlink = 0;

    ledState = false;
}

void WiFiManagerCustom::begin(const char* apName) {

    this->apName = apName;

    pinMode(WIFI_LED_PIN, OUTPUT);
    digitalWrite(WIFI_LED_PIN, LOW);

    Serial.println();
    Serial.println("==============================");
    Serial.println("     ESP32 WIFI MANAGER");
    Serial.println("==============================");

    WiFi.mode(WIFI_STA);

    Serial.println("Mencoba terhubung ke WiFi...");

    if (!wm.autoConnect(this->apName)) {

        Serial.println("Gagal terhubung ke WiFi.");
        Serial.println("Restart ESP32...");

        delay(3000);
        ESP.restart();
    }

    digitalWrite(WIFI_LED_PIN, HIGH);
    ledState = true;

    Serial.println();
    Serial.println("==============================");
    Serial.println("       WIFI TERHUBUNG");
    Serial.println("==============================");

    Serial.print("SSID       : ");
    Serial.println(WiFi.SSID());

    Serial.print("IP Address : ");
    Serial.println(WiFi.localIP());

    Serial.print("RSSI       : ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

    Serial.println("==============================");
}

void WiFiManagerCustom::loop() {

    updateLED();

    if (WiFi.status() == WL_CONNECTED) {
        return;
    }

    unsigned long currentMillis = millis();

    if (currentMillis - lastReconnectAttempt >= RECONNECT_INTERVAL) {

        lastReconnectAttempt = currentMillis;

        reconnect();
    }
}

void WiFiManagerCustom::updateLED() {

    unsigned long currentMillis = millis();

    if (WiFi.status() == WL_CONNECTED) {

        digitalWrite(WIFI_LED_PIN, HIGH);

        ledState = true;

        return;
    }

    if (currentMillis - lastLedBlink >= LED_BLINK_INTERVAL) {

        lastLedBlink = currentMillis;

        ledState = !ledState;

        digitalWrite(WIFI_LED_PIN, ledState);
    }
}

void WiFiManagerCustom::reconnect() {

    Serial.println();
    Serial.println("WiFi terputus!");
    Serial.println("Mencoba reconnect...");

    lastLedBlink = millis();

    WiFi.disconnect();
    WiFi.begin();

    unsigned long startAttempt = millis();

    while (
        WiFi.status() != WL_CONNECTED &&
        millis() - startAttempt < 5000
    ) {

        updateLED();

        delay(100);

        Serial.print(".");
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {

        digitalWrite(WIFI_LED_PIN, HIGH);
        ledState = true;

        Serial.println("WiFi berhasil reconnect!");

        Serial.print("SSID       : ");
        Serial.println(WiFi.SSID());

        Serial.print("IP Address : ");
        Serial.println(WiFi.localIP());

        Serial.print("RSSI       : ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");

    }

    else {

        Serial.println("Reconnect gagal.");
        Serial.println("Akan mencoba kembali dalam 10 detik.");
    }
}

bool WiFiManagerCustom::connected() {

    return WiFi.status() == WL_CONNECTED;
}

String WiFiManagerCustom::getIP() {

    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.localIP().toString();
    }

    return "0.0.0.0";
}

String WiFiManagerCustom::getSSID() {

    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.SSID();
    }

    return "";
}

int WiFiManagerCustom::getRSSI() {

    if (WiFi.status() == WL_CONNECTED) {
        return WiFi.RSSI();
    }

    return 0;
}