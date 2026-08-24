#include "StorageManager.h"
#include <Preferences.h>
#include <time.h>
static Preferences prefs;
String getDateString(){ struct tm timeinfo; if(!getLocalTime(&timeinfo)) return "unknown"; char buffer[20]; strftime(buffer,sizeof(buffer),"%Y-%m-%d",&timeinfo); return String(buffer); }
void saveMinMax(float tempMax,float tempMin){ prefs.begin("tempdata",false); prefs.putFloat("max",tempMax); prefs.putFloat("min",tempMin); prefs.putString("date",getDateString()); prefs.end(); Serial.println("[EEPROM] MinMax Saved"); }
void loadMinMax(float &tempMax,float &tempMin){ prefs.begin("tempdata",true); String savedDate=prefs.getString("date",""); String today=getDateString(); if(savedDate==today && today!="unknown"){tempMax=prefs.getFloat("max",-1000);tempMin=prefs.getFloat("min",1000);Serial.println("[EEPROM] MinMax Restored");}else{tempMax=-1000;tempMin=1000;Serial.println("[EEPROM] New Day Reset");} prefs.end(); }
