#include "OLEDManager.h"

OLEDManager::OLEDManager(
    uint8_t sdaPin,
    uint8_t sclPin,
    uint8_t address
)
    : _sdaPin(sdaPin),
      _sclPin(sclPin),
      _address(address),
      _ready(false),
      _display(
          SCREEN_WIDTH,
          SCREEN_HEIGHT,
          &Wire,
          -1
      )
{
}

bool OLEDManager::begin()
{
    Wire.begin(
        _sdaPin,
        _sclPin
    );

    if (
        !_display.begin(
            SSD1306_SWITCHCAPVCC,
            _address
        )
    )
    {
        Serial.println(
            "OLED gagal ditemukan!"
        );

        _ready = false;

        return false;
    }

    _ready = true;

    Serial.println(
        "OLED OK"
    );

    showStartup();

    return true;
}

void OLEDManager::showStartup()
{
    if (!_ready)
    {
        return;
    }

    _display.clearDisplay();

    _display.setTextColor(
        SSD1306_WHITE
    );

    _display.setTextSize(1);

    _display.setCursor(
        20,
        0
    );

    _display.println(
        "MONITORING OLI"
    );

    _display.display();
}

void OLEDManager::update(
    float temperature,
    float pwmPercent,
    float rpm,
    int servoAngle
)
{
    if (!_ready)
    {
        return;
    }

    _display.clearDisplay();

    _display.setTextColor(
        SSD1306_WHITE
    );

    // =====================================================
    // JUDUL
    // =====================================================

    _display.setTextSize(1);

    _display.setCursor(
        20,
        0
    );

    _display.println(
        "LOKAL MONITORING"
    );

    // Garis pembatas

    _display.drawLine(
        0,
        10,
        127,
        10,
        SSD1306_WHITE
    );

    // =====================================================
    // SUHU
    // =====================================================

    _display.setCursor(
        0,
        16
    );

    _display.print(
        "Suhu   : "
    );

    if (isnan(temperature))
    {
        // Tetap mengikuti
        // tampilan program asli
        _display.println("0");
    }
    else
    {
        _display.print(
            temperature,
            1
        );

        _display.println(
            " C"
        );
    }

    // =====================================================
    // PWM
    // =====================================================

    _display.setCursor(
        0,
        29
    );

    _display.print(
        "PWM    : "
    );

    _display.print(
        pwmPercent,
        1
    );

    _display.println(
        " %"
    );

    // =====================================================
    // SPEED / RPM
    // =====================================================

    _display.setCursor(
        0,
        42
    );

    _display.print(
        "Speed  : "
    );

    _display.print(
        rpm,
        0
    );

    _display.println(
        " RPM"
    );

    // =====================================================
    // SERVO
    // =====================================================

    _display.setCursor(
        0,
        55
    );

    _display.print(
        "Servo  : "
    );

    _display.print(
        servoAngle
    );

    _display.println(
        (char)247
    );

    _display.display();
}

bool OLEDManager::isReady()
{
    return _ready;
}