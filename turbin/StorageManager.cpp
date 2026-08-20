#include "StorageManager.h"

StorageManager::StorageManager()
{
}

void StorageManager::begin()
{
    // Preferences digunakan saat
    // proses read/write berlangsung.
}

String StorageManager::getDateString()
{
    struct tm timeinfo;

    if (!getLocalTime(&timeinfo))
    {
        return "unknown";
    }

    char buffer[20];

    strftime(
        buffer,
        sizeof(buffer),
        "%Y-%m-%d",
        &timeinfo
    );

    return String(buffer);
}

void StorageManager::saveMinMax(
    float tempMax,
    float tempMin
)
{
    _prefs.begin(
        _namespace,
        false
    );

    _prefs.putFloat(
        _keyMax,
        tempMax
    );

    _prefs.putFloat(
        _keyMin,
        tempMin
    );

    _prefs.putString(
        _keyDate,
        getDateString()
    );

    _prefs.end();

    Serial.println(
        "[EEPROM] MinMax Saved"
    );
}

bool StorageManager::loadMinMax(
    float &tempMax,
    float &tempMin
)
{
    _prefs.begin(
        _namespace,
        true
    );

    String savedDate =
        _prefs.getString(
            _keyDate,
            ""
        );

    String today =
        getDateString();

    if (savedDate == today)
    {
        tempMax =
            _prefs.getFloat(
                _keyMax,
                -1000
            );

        tempMin =
            _prefs.getFloat(
                _keyMin,
                1000
            );

        _prefs.end();

        Serial.println(
            "[EEPROM] MinMax Restored"
        );

        return true;
    }

    _prefs.end();

    tempMax = -1000;
    tempMin = 1000;

    Serial.println(
        "[EEPROM] New Day Reset"
    );

    return false;
}

bool StorageManager::isNewDay()
{
    _prefs.begin(
        _namespace,
        true
    );

    String savedDate =
        _prefs.getString(
            _keyDate,
            ""
        );

    _prefs.end();

    String today =
        getDateString();

    return (
        savedDate != today
    );
}