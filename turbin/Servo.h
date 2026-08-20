#ifndef SERVO_H
#define SERVO_H

#include <Arduino.h>

class Servo
{
public:
  Servo();

  bool attach(int pin);
  void detach();

  void write(int angle);
  int read();

private:
  int _pin;
  int _angle;
  bool _attached;

  void setPWM(int angle);
};

#endif
