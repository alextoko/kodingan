#ifndef OLED_MANAGER_H
#define OLED_MANAGER_H
#include <Arduino.h>
void initOLED();
void updateOLED(float temperature, float pwmPercent, float rpm, int servoAngle);
#endif
