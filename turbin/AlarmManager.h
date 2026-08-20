#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H

#include <Arduino.h>

class AlarmManager
{
public:

    AlarmManager(
        uint8_t alarmPin = 19
    );

    void begin();

    void update(
        float temperature,
        float alarmSet
    );

    bool isActive();

private:

    uint8_t _alarmPin;

    bool _alarmState;

    unsigned long _lastBeep;
    bool _beepState;

    void alarmOn();
    void alarmOff();
};

#endif