#include "SensorPT100.h"
#include <math.h>

PT100_420::PT100_420(float resistor, float minTemp, float maxTemp)
    : _resistor(resistor), _minTemp(minTemp), _maxTemp(maxTemp) {}

void PT100_420::begin() {
    ads.setGain(GAIN_ONE);
    ads.begin();
}

float PT100_420::readAll(int16_t &adc, float &voltage, float &current) {
    long total = 0;
    for (int i = 0; i < 5; i++) {
        total += ads.readADC_SingleEnded(0);
        delay(1);
    }
    int16_t raw = total / 5;
    float adcCal = raw * 1.0180f;
    adc = adcCal;
    voltage = adcCal * 0.125f / 1000.0f;
    current = (voltage / _resistor) * 1000.0f;
    float temp = ((current - 4.0f) / 16.0f) * (_maxTemp - _minTemp) + _minTemp;
    if (current < 4 || current > 20.5) return NAN;
    return temp;
}
