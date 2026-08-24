#include <WiFi.h>
#include <math.h>
#include <time.h>
#include "SensorPT100.h"
#include "WiFiManagerCustom.h"
#include "FirebaseManager.h"
#include "MotorManager.h"
#include "ServoManager.h"
#include "AlarmManager.h"
#include "OLEDManager.h"
#include "StorageManager.h"
#include "DataLogger.h"

#define FB_INTERVAL 500
#define SENSOR_INTERVAL 100
#define PRINT_INTERVAL 500
#define OLED_INTERVAL 300

PT100_420 sensor(165.0, 0.0, 100.0);
WiFiManagerCustom wifiManager;
float tempMax=-1000, tempMin=1000;
float tempCurrent=0, tempSum=0, tempAvg=0, alarmSet=65.0;
unsigned long sampleCount=0,lastSensor=0,lastFirebase=0,lastPrint=0,lastOLED=0;
int16_t adcVal=0; float voltageVal=0,currentVal=0; int lastDay=-1;

float round1(float v){return round(v*10.0)/10.0;}
float round3(float v){return round(v*1000.0)/1000.0;}

void setup(){
    Serial.begin(115200);
    initMotor();
    initServo();
    initAlarm();
    wifiManager.begin("ESP32-IoT");
    configTime(0,0,"pool.ntp.org","time.nist.gov");
    Serial.println("NTP started...");
    delay(2000);
    tempMax=-1000; tempMin=1000;
    loadMinMax(tempMax,tempMin);
    initFirebase(alarmSet,wifiManager);
    sensor.begin();
    initOLED();
}

void loop(){
    wifiManager.loop();
    checkFirebaseConnection(wifiManager);
    updateHeartbeat();

    struct tm timeinfo;
    if(getLocalTime(&timeinfo) && timeinfo.tm_mday!=lastDay){
        lastDay=timeinfo.tm_mday; tempMax=-1000; tempMin=1000;
        saveMinMax(tempMax,tempMin);
        Serial.println("[SYSTEM] Daily MinMax Reset");
    }

    updateDeviceIP(wifiManager);
    readAlarmSetpoint(alarmSet);

    if(millis()-lastSensor>=SENSOR_INTERVAL){
        lastSensor=millis();
        tempCurrent=sensor.readAll(adcVal,voltageVal,currentVal);
        if(!isnan(tempCurrent)&&tempCurrent>0&&tempCurrent<100){
            if(tempMax==-1000)tempMax=tempCurrent;
            if(tempMin==1000)tempMin=tempCurrent;
            if(tempCurrent>tempMax)tempMax=tempCurrent;
            if(tempCurrent<tempMin)tempMin=tempCurrent;
            tempSum+=tempCurrent; sampleCount++; tempAvg=round1(tempSum/sampleCount);
        }
    }

    updateAlarm(tempCurrent,alarmSet);
    updateMotorTarget(tempCurrent);
    motorPWMUpdate();
    updateServo(tempCurrent);
    int pwmNow=getMotorPWM();

    if(millis()-lastOLED>=OLED_INTERVAL){lastOLED=millis();updateOLED(tempCurrent,getMotorPWMPercent(),getMotorRPM(),getServoAngle());}

    if(millis()-lastFirebase>=FB_INTERVAL){
        lastFirebase=millis();
        if(isnan(tempCurrent))writeFloat("/Temperature/current",0.0); else writeFloat("/Temperature/current",round1(tempCurrent));
        if(tempMax>0&&tempMax<100)writeFloat("/Temperature/maximal",round1(tempMax));
        if(tempMin>0&&tempMin<100)writeFloat("/Temperature/minimal",round1(tempMin));
        writeAlarmStatus(isAlarmActive());
        writePWM(pwmNow,getMotorPWMPercent());
        writeRPM(getMotorRPM());
        sendToSheetIfChanged(tempCurrent,alarmSet,wifiManager.connected());
    }

    static float lastTempPrint=-999; static int lastPWMPrint=-999; static int16_t lastADCPrint=-999;
    if(millis()-lastPrint>=PRINT_INTERVAL){
        lastPrint=millis();
        bool tempChanged=isnan(tempCurrent)?!isnan(lastTempPrint):(isnan(lastTempPrint)||fabs(tempCurrent-lastTempPrint)>=0.1);
        bool pwmChanged=pwmNow!=lastPWMPrint;
        bool adcChanged=abs(adcVal-lastADCPrint)>=5;
        if(tempChanged||pwmChanged||adcChanged){
            Serial.println(); Serial.println("=========================================="); Serial.println("        MONITORING SUHU OLI PLTA"); Serial.println("==========================================");
            Serial.print("Arus        : ");Serial.print(currentVal,2);Serial.println(" mA");
            Serial.print("Tegangan    : ");Serial.print(round3(voltageVal),3);Serial.println(" V");
            Serial.print("ADC         : ");Serial.println(adcVal);
            Serial.print("Temperatur  : ");if(isnan(tempCurrent))Serial.println("0.0 °C");else{Serial.print(round1(tempCurrent),1);Serial.println(" °C");}
            Serial.print("Suhu Min    : ");if(tempMin>0&&tempMin<100){Serial.print(round1(tempMin),1);Serial.println(" °C");}else Serial.println("-");
            Serial.print("Suhu Max    : ");if(tempMax>0&&tempMax<100){Serial.print(round1(tempMax),1);Serial.println(" °C");}else Serial.println("-");
            Serial.print("Setpoint    : ");Serial.print(round1(alarmSet),1);Serial.println(" °C");
            Serial.print("PWM Motor   : ");Serial.print(pwmNow);Serial.print(" (");Serial.print(round(getMotorPWMPercent()*10.0)/10.0,1);Serial.println("%)");
            Serial.print("Speed       : ");Serial.print(round(getMotorRPM()));Serial.println(" RPM");
            Serial.print("Servo       : ");Serial.print(getServoAngle());Serial.println(" degree");
            Serial.print("Alarm       : ");Serial.println(isAlarmActive()?"ON":"OFF");
            Serial.print("WiFi        : ");Serial.println(wifiManager.connected()?"CONNECTED":"DISCONNECTED");
            Serial.print("Firebase    : ");Serial.println(firebaseIsReady()?"READY":"NOT READY");
            Serial.print("IP          : ");Serial.println(wifiManager.connected()?wifiManager.getIP():"-");
            Serial.println("==========================================");
            lastTempPrint=tempCurrent;lastPWMPrint=pwmNow;lastADCPrint=adcVal;
        }
    }
}
