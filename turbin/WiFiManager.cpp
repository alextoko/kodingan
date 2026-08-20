#include "WiFiManager.h"

WiFiManager::WiFiManager(
    const char* ssid,
    const char* password,
    uint8_t ledPin
)
{
    _ssid = ssid;
    _password = password;
    _ledPin = ledPin;

    _wifiConnected = false;

    _lastReconnect = 0;
    _lastBlink = 0;

    _ledState = false;
}

void WiFiManager::begin()
{
    pinMode(_ledPin, OUTPUT);

    digitalWrite(_ledPin, LOW);

    WiFi.mode(WIFI_STA);

    WiFi.begin(_ssid, _password);

    Serial.print("[WiFi] Connecting");

    unsigned long startAttempt = millis();

    while (
        WiFi.status() != WL_CONNECTED &&
        millis() - startAttempt < 3000
    )
    {
        Serial.print(".");

        delay(500);
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        _wifiConnected = true;

        digitalWrite(_ledPin, HIGH);

        Serial.println();

        Serial.println("[WiFi] Connected");

        Serial.print("[WiFi] IP: ");

        Serial.println(WiFi.localIP());
    }
    else
    {
        _wifiConnected = false;

        Serial.println();

        Serial.println("[WiFi] Failed");
    }
}

void WiFiManager::update()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        _wifiConnected = true;

        digitalWrite(_ledPin, HIGH);
    }
    else
    {
        _wifiConnected = false;

        // LED berkedip setiap 300 ms
        if (millis() - _lastBlink > 300)
        {
            _ledState = !_ledState;

            digitalWrite(_ledPin, _ledState);

            _lastBlink = millis();
        }

        // Reconnect setiap 3 detik
        if (millis() - _lastReconnect > 3000)
        {
            Serial.println("[WiFi] Reconnecting...");

            WiFi.begin(_ssid, _password);

            _lastReconnect = millis();
        }
    }
}

bool WiFiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManager::getIP()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return WiFi.localIP().toString();
    }

    return "";
}