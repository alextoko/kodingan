#include "AlarmManager.h"
#include <math.h>
static bool alarmState = false;
void initAlarm() { pinMode(ALARM_PIN, OUTPUT); digitalWrite(ALARM_PIN, LOW); }
bool isAlarmActive() { return alarmState; }
void updateAlarm(float temp, float alarmSet) {
    static unsigned long lastTime = 0;
    static uint8_t step = 0;
    const uint16_t BEEP_ON=100, BEEP_OFF=300, PAUSE=200;
    if (!isnan(temp) && (temp < 0 || temp > 150)) {
        alarmState=false; digitalWrite(ALARM_PIN,LOW); lastTime=millis(); step=0; return;
    }
    alarmState = (isnan(temp) || temp <= 0.5 || temp >= alarmSet);
    if (!alarmState) { digitalWrite(ALARM_PIN,LOW); step=0; return; }
    unsigned long now=millis();
    switch(step) {
        case 0: digitalWrite(ALARM_PIN,HIGH); if(now-lastTime>=BEEP_ON){lastTime=now;step=1;} break;
        case 1: digitalWrite(ALARM_PIN,LOW); if(now-lastTime>=BEEP_OFF){lastTime=now;step=2;} break;
        case 2: digitalWrite(ALARM_PIN,HIGH); if(now-lastTime>=BEEP_ON){lastTime=now;step=3;} break;
        case 3: digitalWrite(ALARM_PIN,LOW); if(now-lastTime>=BEEP_OFF){lastTime=now;step=4;} break;
        case 4: digitalWrite(ALARM_PIN,HIGH); if(now-lastTime>=BEEP_ON){lastTime=now;step=5;} break;
        case 5: digitalWrite(ALARM_PIN,LOW); if(now-lastTime>=PAUSE){lastTime=now;step=0;} break;
    }
}
