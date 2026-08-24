#ifndef FIREBASE_MANAGER_H
#define FIREBASE_MANAGER_H
#include <Arduino.h>
#include "WiFiManagerCustom.h"
void initFirebase(float alarmDefault, WiFiManagerCustom &wifi);
void checkFirebaseConnection(WiFiManagerCustom &wifi);
bool firebaseIsReady();
void readAlarmSetpoint(float &alarmSet);
void writeFloat(const String &path,float value);
void writeAlarmStatus(bool value);
void writePWM(int pwmValue,float pwmPercent);
void writeRPM(float rpmValue);
void updateDeviceIP(WiFiManagerCustom &wifi);
void updateHeartbeat();
#endif
