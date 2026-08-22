#include "WiFiManagerCustom.h"

WiFiManagerCustom::WiFiManagerCustom()
{
    apName = "ESP32-WIFI-MANAGER";

    lastReconnectAttempt = 0;
    lastLedBlink = 0;

    ledState = false;
    wasConnected = false;
}

void WiFiManagerCustom::begin(const char* apName)
{
    this->apName = apName;

    pinMode(WIFI_LED_PIN, OUTPUT);

    // Start in a known disconnected LED state.
    ledState = false;
    digitalWrite(WIFI_LED_PIN, LOW);

    lastReconnectAttempt = millis();
    lastLedBlink = millis();
    wasConnected = false;

    WiFi.mode(WIFI_STA);

    // Use WiFiManager only for the initial configuration/connection.
    if (!wm.autoConnect(this->apName))
    {
        delay(3000);
        ESP.restart();
    }

    // autoConnect() returned successfully.
    wasConnected = (WiFi.status() == WL_CONNECTED);

    if (wasConnected)
    {
        ledState = true;
        digitalWrite(WIFI_LED_PIN, HIGH);
        lastReconnectAttempt = millis();
        lastLedBlink = millis();
    }
}

void WiFiManagerCustom::loop()
{
    const unsigned long currentMillis = millis();
    const bool wifiConnected = (WiFi.status() == WL_CONNECTED);

    // Handle connection state changes first.
    if (wifiConnected)
    {
        if (!wasConnected)
        {
            // WiFi has just reconnected.
            wasConnected = true;
            ledState = true;
            digitalWrite(WIFI_LED_PIN, HIGH);
            lastLedBlink = currentMillis;
        }

        // Keep the LED continuously ON while connected.
        digitalWrite(WIFI_LED_PIN, HIGH);
        ledState = true;
        lastReconnectAttempt = currentMillis;

        return;
    }

    // WiFi is disconnected.
    if (wasConnected)
    {
        // Enter disconnected state immediately.
        wasConnected = false;
        ledState = false;
        digitalWrite(WIFI_LED_PIN, LOW);
        lastLedBlink = currentMillis;
    }

    // Blink LED while disconnected. This is non-blocking.
    updateLED();

    // Try to reconnect periodically without blocking the main loop.
    if (currentMillis - lastReconnectAttempt >= RECONNECT_INTERVAL)
    {
        lastReconnectAttempt = currentMillis;
        reconnect();
    }
}

void WiFiManagerCustom::updateLED()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        ledState = true;
        digitalWrite(WIFI_LED_PIN, HIGH);
        return;
    }

    const unsigned long currentMillis = millis();

    if (currentMillis - lastLedBlink >= LED_BLINK_INTERVAL)
    {
        lastLedBlink = currentMillis;
        ledState = !ledState;
        digitalWrite(WIFI_LED_PIN, ledState ? HIGH : LOW);
    }
}

void WiFiManagerCustom::reconnect()
{
    // Non-blocking reconnect.
    // Do not wait in a while() loop here so the rest of the program
    // can continue running normally while WiFi is unavailable.
    WiFi.disconnect();
    WiFi.begin();
}

bool WiFiManagerCustom::connected()
{
    return WiFi.status() == WL_CONNECTED;
}

String WiFiManagerCustom::getIP()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return WiFi.localIP().toString();
    }

    return "0.0.0.0";
}

String WiFiManagerCustom::getSSID()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return WiFi.SSID();
    }

    return "";
}

int WiFiManagerCustom::getRSSI()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return WiFi.RSSI();
    }

    return 0;
}
