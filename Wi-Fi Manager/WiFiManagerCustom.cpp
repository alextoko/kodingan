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
    portalActive = false;
    reconnectAttempts = 0;
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
    portalActive = false;
    reconnectAttempts = 0;

    WiFi.mode(WIFI_STA);

    Serial.println("[WiFi] Starting WiFi Manager...");
    Serial.println("[WiFi] Quick startup connection: trying stored WiFi for 10 seconds.");

    // Try the previously stored WiFi credentials first.
    // This avoids waiting for the normal WiFiManager autoConnect timeout.
    if (tryStoredWiFi(STARTUP_CONNECT_TIMEOUT))
    {
        wasConnected = true;
        ledState = true;
        digitalWrite(WIFI_LED_PIN, HIGH);
        lastReconnectAttempt = millis();
        lastLedBlink = millis();
        reconnectAttempts = 0;

        Serial.print("[WiFi] Connected to stored WiFi. IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("[WiFi] SSID: ");
        Serial.println(WiFi.SSID());
        return;
    }

    // Stored WiFi was not available during the startup window.
    // Open the configuration portal so the user can register a new WiFi.
    Serial.println("[WiFi] Stored WiFi not available after 10 seconds.");
    startConfigPortal();
}

bool WiFiManagerCustom::tryStoredWiFi(unsigned long timeoutMs)
{
    // No SSID/SSID scan is required here. WiFi.begin() without arguments
    // asks the ESP32 WiFi stack to use the stored credentials.
    WiFi.disconnect();
    delay(100);
    WiFi.begin();

    const unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs)
    {
        updateLED();
        delay(100);
    }

    return WiFi.status() == WL_CONNECTED;
}

void WiFiManagerCustom::loop()
{
    if (portalActive)
    {
        // startConfigPortal() is blocking until the portal exits.
        // This guard prevents normal reconnect logic from running while
        // the configuration portal is considered active.
        return;
    }

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
            reconnectAttempts = 0;

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
        reconnectAttempts = 0;

        ledState = false;
        digitalWrite(WIFI_LED_PIN, LOW);
        lastLedBlink = currentMillis;

        Serial.println("[WiFi] Disconnected. Starting reconnect cycle.");
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

            Serial.print("[WiFi] Reconnect attempt ");
            Serial.print(reconnectAttempts);
            Serial.println(" timed out.");

            // After the fifth failed attempt, open the configuration portal.
            if (reconnectAttempts >= MAX_RECONNECT_ATTEMPTS)
            {
                startConfigPortal();
            }
        }

        return;
    }

    // Start the next reconnect attempt only after the configured interval.
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

    if (reconnectInProgress)
    {
        return;
    }

    reconnectAttempts++;
    reconnectInProgress = true;
    reconnectStart = currentMillis;
    lastReconnectAttempt = currentMillis;

    Serial.print("[WiFi] Starting reconnect attempt ");
    Serial.print(reconnectAttempts);
    Serial.print("/");
    Serial.println(MAX_RECONNECT_ATTEMPTS);

    // Non-blocking reconnect using the previously stored WiFi credentials.
    WiFi.disconnect();
    WiFi.begin();
}

void WiFiManagerCustom::startConfigPortal()
{
    Serial.println("[WiFi] Opening WiFiManager configuration portal...");

    reconnectInProgress = false;
    reconnectStart = 0;
    reconnectAttempts = 0;
    portalActive = true;

    ledState = false;
    digitalWrite(WIFI_LED_PIN, LOW);

    // The old WiFi credentials are not erased here.
    // If the user does not save a new network, the existing credentials
    // remain available for future reconnect attempts.
    wm.setConfigPortalTimeout(PORTAL_TIMEOUT / 1000UL);

    const bool portalResult = wm.startConfigPortal(this->apName);

    portalActive = false;

    if (portalResult && WiFi.status() == WL_CONNECTED)
    {
        wasConnected = true;
        ledState = true;
        digitalWrite(WIFI_LED_PIN, HIGH);
        lastReconnectAttempt = millis();
        lastLedBlink = millis();
        reconnectAttempts = 0;

        Serial.print("[WiFi] New WiFi configured. IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("[WiFi] SSID: ");
        Serial.println(WiFi.SSID());
    }
    else
    {
        wasConnected = false;
        lastReconnectAttempt = millis();
        lastLedBlink = millis();
        reconnectAttempts = 0;

        Serial.println("[WiFi] Configuration portal closed without a successful connection.");
        Serial.println("[WiFi] Stored credentials were not intentionally cleared.");
    }
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
