#include "ServoManager.h"
#include <math.h>
static Servo myServo;
static int servoAngle=SERVO_MIN_ANGLE;
void initServo(){ myServo.attach(SERVO_PIN); servoAngle=SERVO_MIN_ANGLE; myServo.write(servoAngle); Serial.println("[SERVO] Initialized"); Serial.print("[SERVO] Initial angle: "); Serial.print(servoAngle); Serial.println(" degree"); }
void updateServo(float temperature){
    if(!isnan(temperature) && temperature>75.0){ servoAngle=57; myServo.write(servoAngle); return; }
    if(isnan(temperature)) servoAngle=SERVO_MIN_ANGLE;
    else if(temperature<=SERVO_TEMP_MIN) servoAngle=SERVO_MIN_ANGLE;
    else if(temperature>=SERVO_TEMP_MAX) servoAngle=SERVO_MAX_ANGLE;
    else { float angle=SERVO_MIN_ANGLE+((temperature-SERVO_TEMP_MIN)*(SERVO_MAX_ANGLE-SERVO_MIN_ANGLE)/(SERVO_TEMP_MAX-SERVO_TEMP_MIN)); servoAngle=round(angle); }
    servoAngle=constrain(servoAngle,SERVO_MIN_ANGLE,SERVO_MAX_ANGLE); myServo.write(servoAngle);
}
int getServoAngle(){return servoAngle;}
