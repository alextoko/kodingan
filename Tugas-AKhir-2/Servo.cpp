#include "Servo.h"
Servo::Servo(){_pin=-1;_angle=90;_attached=false;}
bool Servo::attach(int pin){_pin=pin;ledcAttach(_pin,50,16);_attached=true;write(90);return true;}
void Servo::detach(){if(_attached){ledcDetach(_pin);_attached=false;}}
void Servo::write(int angle){if(!_attached)return;angle=constrain(angle,0,180);_angle=angle;setPWM(angle);}
int Servo::read(){return _angle;}
void Servo::setPWM(int angle){const uint32_t MIN_US=500,MAX_US=2500,PERIOD_US=20000,MAX_DUTY=65535;uint32_t pulseWidth=MIN_US+((uint32_t)angle*(MAX_US-MIN_US)/180UL);uint32_t duty=(pulseWidth*MAX_DUTY)/PERIOD_US;ledcWrite(_pin,duty);}
