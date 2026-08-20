#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>

class StorageManager
{
public:

    StorageManager();

    void begin();

    void saveMinMax(
        float tempMax,
        float tempMin
    );

    bool loadMinMax(
        float &tempMax,
        float &tempMin
    );

    bool isNewDay();

    String getDateString();

private:

    Preferences _prefs;

    const char* _namespace = "tempdata";
    const char* _keyMax = "max";
    const char* _keyMin = "min";
    const char* _keyDate = "date";
};

#endif