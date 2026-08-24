#include "MotorManager.h"
#include <math.h>

static int currentPWM = 0;
static int targetPWM = 0;
static float currentRPM = 0;
static float pwmPercentActual = 60.0;

void initMotor() {
    pinMode(MOTOR_PWM_PIN, OUTPUT);
    digitalWrite(MOTOR_PWM_PIN, HIGH);
    delay(50);
    ledcAttach(MOTOR_PWM_PIN, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
    currentPWM = 0;
    targetPWM = 0;
    ledcWrite(MOTOR_PWM_PIN, 255);
}

void updateMotorTarget(float tempRaw) {
    if (!isnan(tempRaw) && tempRaw > 75.0) {
        pwmPercentActual = 0.0;
        targetPWM = 0;
        return;
    }
    if (isnan(tempRaw) || tempRaw <= 0 || tempRaw > TEMP_MAX_VALID) {
        pwmPercentActual = 60.0;
        targetPWM = round((60.0 / 100.0) * PWM_MAX);
        targetPWM = constrain(targetPWM, 0, PWM_MAX);
        return;
    }
    const float tempMin = 30.0, tempMax = 65.0;
    const float pwmMin = 60.0, pwmMax = 100.0;
    float pwmPercent;
    if (tempRaw <= tempMin) pwmPercent = pwmMin;
    else if (tempRaw >= tempMax) pwmPercent = pwmMax;
    else pwmPercent = pwmMin + ((tempRaw - tempMin) * (pwmMax - pwmMin) / (tempMax - tempMin));
    pwmPercentActual = pwmPercent;
    targetPWM = round((pwmPercent / 100.0) * PWM_MAX);
    targetPWM = constrain(targetPWM, 0, PWM_MAX);
}

void motorPWMUpdate() {
    currentPWM = constrain(targetPWM, 0, PWM_MAX);
    if (currentPWM == 0) {
        ledcWrite(MOTOR_PWM_PIN, PWM_MAX);
        digitalWrite(MOTOR_PWM_PIN, HIGH);
        currentRPM = 0;
        return;
    }
    uint8_t pwmOut = PWM_MAX - currentPWM;
    ledcWrite(MOTOR_PWM_PIN, pwmOut);
    currentRPM = (pwmPercentActual / 100.0) * 1430.0;
}

int getMotorPWM() { return currentPWM; }
float getMotorRPM() { return currentRPM; }
float getMotorPWMPercent() { return pwmPercentActual; }
