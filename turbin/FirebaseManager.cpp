#include "FirebaseManager.h"

FirebaseManager::FirebaseManager(
    const char* apiKey,
    const char* databaseURL
)
{
    _apiKey = apiKey;
    _databaseURL = databaseURL;

    _firebaseReady = false;
    _firebaseStarted = false;

    _lastAlarmSet = -999;
    _lastAlarmStatus = false;
    _lastPWM = -1;
    _lastIP = "";

    _lastHeartbeat = 0;
}

void FirebaseManager::begin(float alarmDefault)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        return;
    }

    _config.api_key = _apiKey;

    _config.database_url = _databaseURL;

    if (!Firebase.signUp(
            &_config,
            &_auth,
            "",
            ""
        ))
    {
        Serial.printf(
            "[Firebase] Signup failed: %s\n",
            _config.signer.signupError.message.c_str()
        );

        _firebaseReady = false;

        return;
    }

    Firebase.begin(
        &_config,
        &_auth
    );

    Firebase.reconnectWiFi(true);

    _firebaseReady = true;

    _firebaseStarted = true;

    Serial.println("[Firebase] Connected");

    // Jika setpoint belum ada,
    // buat menggunakan nilai default
    if (!Firebase.RTDB.get(
            &_fbdo,
            "/Set/alarm_on"
        ))
    {
        Firebase.RTDB.setFloat(
            &_fbdo,
            "/Set/alarm_on",
            alarmDefault
        );
    }
}

void FirebaseManager::readAlarmSetpoint(
    float &alarmSet
)
{
    if (
        !_firebaseReady ||
        !Firebase.ready()
    )
    {
        return;
    }

    if (
        Firebase.RTDB.get(
            &_fbdo,
            "/Set/alarm_on"
        )
    )
    {
        float v;

        if (_fbdo.dataType() == "string")
        {
            v = _fbdo.stringData().toFloat();
        }
        else
        {
            v = _fbdo.to<float>();
        }

        if (
            !isnan(v) &&
            v > 0 &&
            v < 200
        )
        {
            if (v != _lastAlarmSet)
            {
                alarmSet = v;

                _lastAlarmSet = v;

                Serial.print(
                    "🔥 SETPOINT UPDATE: "
                );

                Serial.println(alarmSet);
            }
        }
    }
}

void FirebaseManager::writeFloat(
    const String &path,
    float value
)
{
    if (
        !_firebaseReady ||
        !Firebase.ready()
    )
    {
        return;
    }

    Firebase.RTDB.setFloat(
        &_fbdo,
        path.c_str(),
        value
    );
}

void FirebaseManager::writeAlarmStatus(
    bool value
)
{
    if (
        !_firebaseReady ||
        !Firebase.ready()
    )
    {
        return;
    }

    if (_lastAlarmStatus != value)
    {
        Firebase.RTDB.setBool(
            &_fbdo,
            "/Status/alarm",
            value
        );

        _lastAlarmStatus = value;
    }
}

void FirebaseManager::writePWM(
    int pwmValue,
    float pwmPercentActual
)
{
    if (
        !_firebaseReady ||
        !Firebase.ready()
    )
    {
        return;
    }

    if (_lastPWM != pwmValue)
    {
        Firebase.RTDB.setInt(
            &_fbdo,
            "/Status/pwm_motor",
            pwmValue
        );

        float percent =
            round(
                pwmPercentActual * 10.0
            ) / 10.0;

        Firebase.RTDB.setFloat(
            &_fbdo,
            "/Status/pwm_percent",
            percent
        );

        _lastPWM = pwmValue;
    }
}

void FirebaseManager::updateDeviceIP()
{
    if (
        !_firebaseReady ||
        !Firebase.ready()
    )
    {
        return;
    }

    if (
        WiFi.status() != WL_CONNECTED
    )
    {
        return;
    }

    String ip =
        WiFi.localIP().toString();

    if (ip != _lastIP)
    {
        Firebase.RTDB.setString(
            &_fbdo,
            "/Device/ip",
            ip
        );

        _lastIP = ip;
    }
}

void FirebaseManager::updateHeartbeat()
{
    if (
        !_firebaseReady ||
        !Firebase.ready()
    )
    {
        return;
    }

    if (
        millis() - _lastHeartbeat > 5000
    )
    {
        time_t now = time(nullptr);

        Firebase.RTDB.setInt(
            &_fbdo,
            "/Status/last_update",
            now * 1000
        );

        _lastHeartbeat = millis();
    }
}

bool FirebaseManager::isReady()
{
    return (
        _firebaseReady &&
        Firebase.ready()
    );
}

bool FirebaseManager::isStarted()
{
    return _firebaseStarted;
}