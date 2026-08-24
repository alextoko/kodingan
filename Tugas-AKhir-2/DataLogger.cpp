#include "DataLogger.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <math.h>
#define SEND_DELTA_TEMP 0.1
static const char* SCRIPT_URL="https://script.google.com/macros/s/AKfycbz4entNHRnsf8rgvCbZUx00GVo2W-5X7HmNqUvF8AMXOmT1z25z1Omj_R9UMgmB2OO3vQ/exec";
void sendToSheetIfChanged(float temperature,float alarmSet,bool wifiConnected){
 if(!wifiConnected||isnan(temperature)) return;
 static float lastTemp=-9999; static unsigned long lastSend=0; unsigned long now=millis();
 bool waktu=(now-lastSend>=60000); bool perubahan=(lastTemp<-9000)||(fabs(temperature-lastTemp)>=SEND_DELTA_TEMP); if(!(waktu||perubahan)) return;
 HTTPClient http; http.setConnectTimeout(3000); http.setTimeout(3000); if(!http.begin(SCRIPT_URL)){Serial.println("[Sheet] HTTP begin failed");return;}
 http.addHeader("Content-Type","application/json"); String payload="{\"temperature\":"+String(temperature,1)+",\"alarm\":"+String(alarmSet,1)+"}"; int httpCode=http.POST(payload);
 if(httpCode>=200&&httpCode<300){lastTemp=temperature;lastSend=now;Serial.print("[Sheet] Sent OK: ");Serial.println(httpCode);}else{Serial.print("[Sheet] POST failed: ");Serial.println(httpCode);} http.end();
}
