#include "MotorManager.h"

MotorManager::MotorManager(
    uint8_t pwmPin,
    uint32_t pwmFreq,
    uint8_t pwmResolution
)
{
    _pwmPin = pwmPin;
    _pwmFreq = pwmFreq;
    _pwmResolution = pwmResolution;

    _currentPWM = 60.0;
    _targetPWM = 60.0;

    _pwmPercentActual = 60.0;

    _currentRPM = 0.0;
}

void MotorManager::begin()
{
    ledcSetup(
        0,
        _pwmFreq,
        _pwmResolution
    );

    ledcAttachPin(
        _pwmPin,
        0
    );

    int pwmOut =
        PWM_MAX - (int)_currentPWM;

    ledcWrite(
        0,
        pwmOut
    );

    _currentRPM =
        (_pwmPercentActual / 100.0) *
        1430.0;
}

void MotorManager::update(float temperature)
{
    // =====================================================
    // CONTROL MOTOR BERDASARKAN SUHU
    //
    // Suhu <= 30 C  -> PWM 60%
    // Suhu 30-65 C  -> PWM 60-100%
    // Suhu >= 65 C  -> PWM 100%
    // =====================================================

    if (isnan(temperature))
    {
        return;
    }

    if (temperature <= 30.0)
    {
        _targetPWM = 60.0;
    }
    else if (temperature >= 65.0)
    {
        _targetPWM = 100.0;
    }
    else
    {
        _targetPWM =
            60.0 +
            (
                (temperature - 30.0) /
                (65.0 - 30.0)
            ) *
            (100.0 - 60.0);
    }

    updatePWM();
}

void MotorManager::updatePWM()
{
    // =====================================================
    // PERUBAHAN PWM BERTAHAP
    // =====================================================

    if (_currentPWM < _targetPWM)
    {
        _currentPWM += 1.0;

        if (_currentPWM > _targetPWM)
        {
            _currentPWM = _targetPWM;
        }
    }
    else if (_currentPWM > _targetPWM)
    {
        _currentPWM -= 1.0;

        if (_currentPWM < _targetPWM)
        {
            _currentPWM = _targetPWM;
        }
    }

    // Batasi 0-100%
    if (_currentPWM < 0)
    {
        _currentPWM = 0;
    }

    if (_currentPWM > 100)
    {
        _currentPWM = 100;
    }

    _pwmPercentActual = _currentPWM;

    // Konversi persen ke nilai PWM
    int pwmValue =
        (_currentPWM / 100.0) *
        PWM_MAX;

    // =====================================================
    // LOGIKA PWM AKTIF-LOW
    //
    // Program asli menggunakan:
    //
    // PWM_MAX - currentPWM
    //
    // =====================================================

    int pwmOut =
        PWM_MAX - pwmValue;

    if (pwmOut < 0)
    {
        pwmOut = 0;
    }

    if (pwmOut > PWM_MAX)
    {
        pwmOut = PWM_MAX;
    }

    ledcWrite(
        0,
        pwmOut
    );

    // =====================================================
    // HITUNG RPM
    // Motor maksimum = 1430 RPM
    // =====================================================

    _currentRPM =
        (_pwmPercentActual / 100.0) *
        1430.0;
}

int MotorManager::getCurrentPWM()
{
    return (int)_currentPWM;
}

int MotorManager::getTargetPWM()
{
    return (int)_targetPWM;
}

float MotorManager::getPWMPercent()
{
    return _pwmPercentActual;
}

float MotorManager::getRPM()
{
    return _currentRPM;
}