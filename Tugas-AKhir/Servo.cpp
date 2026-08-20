#include "Servo.h"

Servo::Servo()
{
  _pin = -1;
  _angle = 90;
  _attached = false;
}

bool Servo::attach(int pin)
{
  _pin = pin;

  // Standard hobby servo: 50 Hz, 16-bit resolution.
  ledcAttach(_pin, 50, 16);

  _attached = true;

  // Start at 90 degrees; the main program changes it to 50 degrees.
  write(90);

  return true;
}

void Servo::detach()
{
  if (_attached)
  {
    ledcDetach(_pin);
    _attached = false;
  }
}

void Servo::write(int angle)
{
  if (!_attached)
    return;

  angle = constrain(angle, 0, 180);
  _angle = angle;

  setPWM(angle);
}

int Servo::read()
{
  return _angle;
}

void Servo::setPWM(int angle)
{
  // 50 Hz => 20,000 us period.
  // Typical servo pulse range: 500-2500 us.
  const uint32_t MIN_US = 500;
  const uint32_t MAX_US = 2500;
  const uint32_t PERIOD_US = 20000;
  const uint32_t MAX_DUTY = 65535;

  uint32_t pulseWidth =
    MIN_US +
    ((uint32_t)angle * (MAX_US - MIN_US) / 180UL);

  uint32_t duty =
    (pulseWidth * MAX_DUTY) / PERIOD_US;

  ledcWrite(_pin, duty);
}
