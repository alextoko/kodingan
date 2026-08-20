#include "SensorPT100.h"

SensorPT100::SensorPT100(
    float resistor,
    float minTemp,
    float maxTemp
)
    : _resistor(resistor),
      _minTemp(minTemp),
      _maxTemp(maxTemp)
{
}

void SensorPT100::begin()
{
    ads.setGain(GAIN_ONE);
    ads.begin();
}

float SensorPT100::readAll(
    int16_t &adc,
    float &voltage,
    float &current
)
{
    long total = 0;

    // Ambil 5 sampel ADC
    for (int i = 0; i < 5; i++)
    {
        total += ads.readADC_SingleEnded(0);

        // Tetap dipertahankan sesuai program asli
        delay(1);
    }

    // Rata-rata ADC
    int16_t raw = total / 5;

    // Kalibrasi ADC
    float adcCal = raw * 1.0180f;

    adc = adcCal;

    // Konversi ADC menjadi tegangan
    // GAIN_ONE = 0.125 mV/bit
    voltage = adcCal * 0.125f / 1000.0f;

    // Konversi tegangan menjadi arus
    // R = 165 ohm
    current = (voltage / _resistor) * 1000.0f;

    // Konversi arus 4-20 mA menjadi suhu
    float temp =
        ((current - 4.0f) / 16.0f) *
        (_maxTemp - _minTemp) +
        _minTemp;

    // Validasi sinyal 4-20 mA
    if (current < 4.0f || current > 20.5f)
    {
        return NAN;
    }

    return temp;
}