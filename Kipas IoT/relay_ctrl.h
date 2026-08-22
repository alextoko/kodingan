#ifndef RELAY_CTRL_H
#define RELAY_CTRL_H

#include <Arduino.h>
#include "eeprom_store.h"
#include "SinricPro.h"
#include "SinricProSwitch.h"

/* ===== KONFIG ===== */
static const uint8_t RELAY_COUNT = 3;
static const uint8_t relayPin[RELAY_COUNT] = {16, 17, 18};

/* ===== STATE ===== */
static bool relayState[RELAY_COUNT] = {0, 0, 0};

/* ===== EXCLUSIVE STATE ===== */
static bool exclusivePending = false;
static int exclusiveTarget = -1;
static uint32_t exclusiveTimer = 0;

/* =====================================================
   INIT RELAY + RESTORE DARI EEPROM
   ===================================================== */
inline void initRelay() {
  eepromStoreBegin();
  delay(200); // anti flicker saat boot

  for (int i = 0; i < RELAY_COUNT; i++) {
    pinMode(relayPin[i], OUTPUT);

    relayState[i] = eepromReadBool(i, false);
    digitalWrite(relayPin[i], relayState[i] ? HIGH : LOW);
  }
}

/* =====================================================
   RELAY CONTROL (SATU-SATUNYA AKSES)
   ===================================================== */
inline void setRelay(int index, bool on) {
  digitalWrite(relayPin[index], on ? HIGH : LOW);
  eepromWriteBool(index, on);
  relayState[index] = on;
}

/* =====================================================
   GET STATE (UNTUK SINRIC SYNC)
   ===================================================== */
inline bool getRelayState(int index) {
  return relayState[index];
}

/* =====================================================
   EXCLUSIVE REQUEST (DARI SINRIC)
   ===================================================== */
inline void requestExclusive(int index, bool on,
                             const char* switchID[]) {
  if (!on) {
    setRelay(index, false);
    return;
  }

  exclusivePending = true;
  exclusiveTarget  = index;
  exclusiveTimer   = millis();

  // Matikan semua relay dulu
  for (int i = 0; i < RELAY_COUNT; i++) {
    setRelay(i, false);
  }
}

/* =====================================================
   EXCLUSIVE HANDLER
   ===================================================== */
inline void handleExclusiveTask(const char* switchID[]) {
  if (!exclusivePending) return;

  if (millis() - exclusiveTimer >= 500) {
    setRelay(exclusiveTarget, true);

    // Sinkron ke Google Home
    for (int i = 0; i < RELAY_COUNT; i++) {
      SinricProSwitch &sw = SinricPro[switchID[i]];
      sw.sendPowerStateEvent(relayState[i]);
    }

    exclusivePending = false;
  }
}

#endif
