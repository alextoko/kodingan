#include "AlarmManager.h"

AlarmManager::AlarmManager(
    uint8_t alarmPin
)
{
    _alarmPin = alarmPin;

    _alarmState = false;

    _lastBeep = 0;
    _beepState = false;
}

void AlarmManager::begin()
{
    pinMode(
        _alarmPin,
        OUTPUT
    );

    digitalWrite(
        _alarmPin,
        LOW
    );
}

void AlarmManager::update(
    float temperature,
    float alarmSet
)
{
    // =====================================================
    // LOGIKA ALARM
    //
    // Alarm aktif jika:
    //
    // 1. Sensor invalid / NAN
    // 2. Suhu <= 0.5 C
    // 3. Suhu >= setpoint alarm
    // =====================================================

    _alarmState =
        isnan(temperature) ||
        temperature <= 0.5 ||
        temperature >= alarmSet;


    // =====================================================
    // JIKA ALARM AKTIF
    // =====================================================

    if (_alarmState)
    {
        // Pola bunyi alarm
        // ON/OFF setiap 500 ms

        if (millis() - _lastBeep >= 500)
        {
            _beepState = !_beepState;

            digitalWrite(
                _alarmPin,
                _beepState
            );

            _lastBeep = millis();
        }
    }

    // =====================================================
    // JIKA ALARM TIDAK AKTIF
    // =====================================================

    else
    {
        _beepState = false;

        digitalWrite(
            _alarmPin,
            LOW
        );
    }
}

bool AlarmManager::isActive()
{
    return _alarmState;
}

void AlarmManager::alarmOn()
{
    digitalWrite(
        _alarmPin,
        HIGH
    );
}

void AlarmManager::alarmOff()
{
    digitalWrite(
        _alarmPin,
        LOW
    );
}