#ifndef SERVO_MANAGER_H
#define SERVO_MANAGER_H
#include "Servo.h"
#define SERVO_PIN 18
#define SERVO_MIN_ANGLE 50
#define SERVO_MAX_ANGLE 90
#define SERVO_TEMP_MIN 30.0
#define SERVO_TEMP_MAX 65.0
void initServo();
void updateServo(float temperature);
int getServoAngle();
#endif
