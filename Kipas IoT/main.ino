#include "WiFi_AP.h"
#include "OTA.h"
#include "Time_NTP.h"
#include "relay_ctrl.h"
#include "sinric_ctrl.h"
#include "scheduler.h"

bool sinricReady = false;

void setup() {
  Serial.begin(115200);
  delay(100);

  initRelay();     // restore relay dari EEPROM (wrapper)
  initWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    initNTP();
    initOTA();
    initSinric();
    sinricReady = true;
  }
}

void loop() {
  wifiStatusLED();

  if (sinricReady) {
    handleExclusiveTask(switchID);
    handleSinric();
  }

  handleOTA();
  handleAutoOffAtMidnight(switchID);
}
