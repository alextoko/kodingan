#ifndef OLED_MANAGER_H
#define OLED_MANAGER_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class OLEDManager
{
public:

    OLEDManager(
        uint8_t sdaPin = 21,
        uint8_t sclPin = 22,
        uint8_t address = 0x3C
    );

    bool begin();

    void showStartup();

    void update(
        float temperature,
        float pwmPercent,
        float rpm,
        int servoAngle
    );

    bool isReady();

private:

    static const uint8_t SCREEN_WIDTH = 128;
    static const uint8_t SCREEN_HEIGHT = 64;

    uint8_t _sdaPin;
    uint8_t _sclPin;
    uint8_t _address;

    bool _ready;

    Adafruit_SSD1306 _display;
};

#endif