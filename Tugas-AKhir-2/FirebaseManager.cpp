#include "FirebaseManager.h"
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <time.h>
#include <math.h>
#define API_KEY "AIzaSyAtebbp9XVTRpJROF4mo8FDM50l7vSRJQo"
#define DATABASE_URL "https://oil-plta-bta-default-rtdb.firebaseio.com/"
static FirebaseData fbdo; static FirebaseAuth auth; static FirebaseConfig config; static bool firebaseReady=false; static bool firebaseStarted=false;
bool firebaseIsReady(){return firebaseReady;}
void initFirebase(float alarmDefault,WiFiManagerCustom &wifi){
 if(!wifi.connected()) return; config.api_key=API_KEY;config.database_url=DATABASE_URL;
 if(!Firebase.signUp(&config,&auth,"","")){Serial.printf("[Firebase] Signup failed: %s\n",config.signer.signupError.message.c_str());firebaseReady=false;firebaseStarted=false;return;}
 Firebase.begin(&config,&auth);Firebase.reconnectWiFi(true);firebaseReady=true;firebaseStarted=true;Serial.println("[Firebase] Connected");
 if(Firebase.RTDB.get(&fbdo,"/Set/alarm_on")) Serial.println("[Firebase] Initial setpoint read OK"); else {Firebase.RTDB.setFloat(&fbdo,"/Set/alarm_on",alarmDefault);Serial.println("[Firebase] Default setpoint written");}
}
void checkFirebaseConnection(WiFiManagerCustom &wifi){
 if(!wifi.connected()){if(firebaseReady||firebaseStarted){firebaseReady=false;firebaseStarted=false;Serial.println("[Firebase] WiFi disconnected - waiting for reconnect");}return;}
 if(!firebaseStarted){initFirebase(65.0,wifi);return;} firebaseReady=Firebase.ready();
}
void readAlarmSetpoint(float &alarmSet){
 static float lastSet=-999; static unsigned long lastRead=0; if(!firebaseReady||!Firebase.ready()||millis()-lastRead<1000)return;lastRead=millis();
 if(Firebase.RTDB.get(&fbdo,"/Set/alarm_on")){float v=(fbdo.dataType()=="string")?fbdo.stringData().toFloat():fbdo.to<float>();if(!isnan(v)&&v>0&&v<200&&v!=lastSet){alarmSet=v;lastSet=v;Serial.print("[Firebase] SETPOINT UPDATE: ");Serial.println(alarmSet);}}
 else {static unsigned long lastErrorPrint=0;if(millis()-lastErrorPrint>=5000){lastErrorPrint=millis();Serial.print("[Firebase] Setpoint read failed: ");Serial.println(fbdo.errorReason());}}
}
void writeFloat(const String &path,float value){if(!firebaseReady||!Firebase.ready())return;if(!Firebase.RTDB.setFloat(&fbdo,path.c_str(),value)){static unsigned long lastErrorPrint=0;if(millis()-lastErrorPrint>=5000){lastErrorPrint=millis();Serial.print("[Firebase] Write failed ");Serial.print(path);Serial.print(": ");Serial.println(fbdo.errorReason());}}}
void writeAlarmStatus(bool value){static bool last=false,initialized=false;if(!firebaseReady||!Firebase.ready())return;if(!initialized||last!=value){if(Firebase.RTDB.setBool(&fbdo,"/Status/alarm",value)){last=value;initialized=true;}}}
void writePWM(int pwmValue,float pwmPercent){static int last=-1;if(!firebaseReady||!Firebase.ready())return;if(last!=pwmValue){bool ok1=Firebase.RTDB.setInt(&fbdo,"/Status/pwm_motor",pwmValue);float percent=round(pwmPercent*10.0)/10.0;bool ok2=Firebase.RTDB.setFloat(&fbdo,"/Status/pwm_percent",percent);if(ok1&&ok2)last=pwmValue;}}
void writeRPM(float rpmValue){static int lastRPM=-1;int rpmRounded=round(rpmValue);if(!firebaseReady||!Firebase.ready())return;if(rpmRounded!=lastRPM){if(Firebase.RTDB.setInt(&fbdo,"/Status/rpm_motor",rpmRounded))lastRPM=rpmRounded;}}
void updateDeviceIP(WiFiManagerCustom &wifi){static String lastIP="";if(!firebaseReady||!Firebase.ready()||!wifi.connected())return;String ip=wifi.getIP();if(ip!=lastIP){if(Firebase.RTDB.setString(&fbdo,"/Device/ip",ip))lastIP=ip;}}
void updateHeartbeat(){static unsigned long lastSend=0;if(!firebaseReady||!Firebase.ready())return;if(millis()-lastSend>=5000){time_t now=time(nullptr);if(Firebase.RTDB.setInt(&fbdo,"/Status/last_update",now*1000))lastSend=millis();}}
