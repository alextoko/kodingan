#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H
#include <Arduino.h>
#define ALARM_PIN 19
void initAlarm();
void updateAlarm(float temp, float alarmSet);
bool isAlarmActive();
#endif
