#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H
#include <Arduino.h>
void saveMinMax(float tempMax, float tempMin);
void loadMinMax(float &tempMax, float &tempMin);
String getDateString();
#endif
