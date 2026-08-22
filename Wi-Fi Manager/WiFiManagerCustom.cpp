#include "WiFiManagerCustom.h"

WiFiManagerCustom::WiFiManagerCustom()
{
    apName = "ESP32-WIFI-MANAGER";

    lastReconnectAttempt = 0;
    lastLedBlink = 0;
    reconnectStart = 0;

    ledState = false;
    wasConnected = false;
    reconnectInProgress = false;
}

void WiFiManagerCustom::begin(const char* apName)
{
    this->apName = apName;

    pinMode(WIFI_LED_PIN, OUTPUT);

    ledState = false;
    digitalWrite(WIFI_LED_PIN, LOW);

    lastReconnectAttempt = millis();
    lastLedBlink = millis();
    reconnectStart = 0;
    wasConnected = false;
    reconnectInProgress = false;

    WiFi.mode(WIFI_STA);

    Serial.println("[WiFi] Starting WiFiManager...");

    // WiFiManager is used only for the initial configuration/connection.
    if (!wm.autoConnect(this->apName))
    {
        Serial.println("[WiFi] Initial connection/configuration failed. Restarting...");
        delay(3000);
        ESP.restart();
    }

    wasConnected = (WiFi.status() == WL_CONNECTED);

    if (wasConnected)
    {
        ledState = true;
        digitalWrite(WIFI_LED_PIN, HIGH);
        lastReconnectAttempt = millis();
        lastLedBlink = millis();

        Serial.print("[WiFi] Connected. IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("[WiFi] SSID: ");
        Serial.println(WiFi.SSID());
    }
}

void WiFiManagerCustom::loop()
{
    const unsigned long currentMillis = millis();
    const bool wifiConnected = (WiFi.status() == WL_CONNECTED);

    // WiFi has connected or reconnected.
    if (wifiConnected)
    {
        if (!wasConnected)
        {
            wasConnected = true;
            reconnectInProgress = false;
            reconnectStart = 0;

            ledState = true;
            digitalWrite(WIFI_LED_PIN, HIGH);
            lastLedBlink = currentMillis;

            Serial.print("[WiFi] Reconnected. IP: ");
            Serial.println(WiFi.localIP());
        }

        digitalWrite(WIFI_LED_PIN, HIGH);
        ledState = true;
        reconnectInProgress = false;
        reconnectStart = 0;
        lastReconnectAttempt = currentMillis;

        return;
    }

    // WiFi has just disconnected.
    if (wasConnected)
    {
        wasConnected = false;
        reconnectInProgress = false;
        reconnectStart = 0;

        ledState = false;
        digitalWrite(WIFI_LED_PIN, LOW);
        lastLedBlink = currentMillis;

        Serial.println("[WiFi] Disconnected.");
    }

    // Blink LED while disconnected. This is non-blocking.
    updateLED();

    // If a reconnect is currently in progress, wait for either
    // a successful connection or the non-blocking timeout.
    if (reconnectInProgress)
    {
        if (currentMillis - reconnectStart >= RECONNECT_TIMEOUT)
        {
            reconnectInProgress = false;
            reconnectStart = 0;
            lastReconnectAttempt = currentMillis;

            Serial.println("[WiFi] Reconnect attempt timed out.");
        }

        return;
    }

    // Start a new reconnect attempt only after the configured interval.
    if (currentMillis - lastReconnectAttempt >= RECONNECT_INTERVAL)
    {
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
    const unsigned long currentMillis = millis();

    // Prevent duplicate reconnect attempts while one is still running.
    if (reconnectInProgress)
    {
        return;
    }

    reconnectInProgress = true;
    reconnectStart = currentMillis;
    lastReconnectAttempt = currentMillis;

    Serial.println("[WiFi] Starting reconnect attempt...");

    // Non-blocking reconnect using the previously stored WiFi credentials.
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
