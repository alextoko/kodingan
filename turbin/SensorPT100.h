#ifndef SENSOR_PT100_H
#define SENSOR_PT100_H

#include <Arduino.h>
#include <Adafruit_ADS1X15.h>
#include <math.h>

class SensorPT100
{
public:

    SensorPT100(
        float resistor,
        float minTemp,
        float maxTemp
    );

    void begin();

    float readAll(
        int16_t &adc,
        float &voltage,
        float &current
    );

private:

    Adafruit_ADS1115 ads;

    float _resistor;
    float _minTemp;
    float _maxTemp;
};

#endif