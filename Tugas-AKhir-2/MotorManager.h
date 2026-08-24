#ifndef MOTOR_MANAGER_H
#define MOTOR_MANAGER_H
#include <Arduino.h>

#define MOTOR_PWM_PIN 23
#define MOTOR_PWM_FREQ 1000
#define MOTOR_PWM_RES 8
#define PWM_MAX 255
#define TEMP_MAX_VALID 100

void initMotor();
void updateMotorTarget(float tempRaw);
void motorPWMUpdate();
int getMotorPWM();
float getMotorRPM();
float getMotorPWMPercent();
#endif
