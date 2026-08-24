#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H
#include <Arduino.h>
void sendToSheetIfChanged(float temperature, float alarmSet, bool wifiConnected);
#endif
