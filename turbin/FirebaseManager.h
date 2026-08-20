#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

class FirebaseManager
{
public:

    FirebaseManager(
        const char* apiKey,
        const char* databaseURL
    );

    void begin(float alarmDefault);

    void readAlarmSetpoint(float &alarmSet);

    void writeFloat(
        const String &path,
        float value
    );

    void writeAlarmStatus(
        bool value
    );

    void writePWM(
        int pwmValue,
        float pwmPercentActual
    );

    void updateDeviceIP();

    void updateHeartbeat();

    bool isReady();

    bool isStarted();

private:

    FirebaseData _fbdo;
    FirebaseAuth _auth;
    FirebaseConfig _config;

    const char* _apiKey;
    const char* _databaseURL;

    bool _firebaseReady;
    bool _firebaseStarted;

    float _lastAlarmSet;
    bool _lastAlarmStatus;
    int _lastPWM;
    String _lastIP;

    unsigned long _lastHeartbeat;
};

#endif