#ifndef MOTOR_MANAGER_H
#define MOTOR_MANAGER_H

#include <Arduino.h>

class MotorManager
{
public:

    MotorManager(
        uint8_t pwmPin = 23,
        uint32_t pwmFreq = 1000,
        uint8_t pwmResolution = 8
    );

    void begin();

    void update(float temperature);

    int getCurrentPWM();

    int getTargetPWM();

    float getPWMPercent();

    float getRPM();

private:

    uint8_t _pwmPin;
    uint32_t _pwmFreq;
    uint8_t _pwmResolution;

    const int PWM_MAX = 255;

    float _currentPWM;
    float _targetPWM;

    float _pwmPercentActual;
    float _currentRPM;

    void updatePWM();
};

#endif