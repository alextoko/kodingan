#ifndef SINRIC_CTRL_H
#define SINRIC_CTRL_H

#include <Arduino.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"
#include "relay_ctrl.h"

#define APP_KEY           "d43d4bd8-0241-4997-a46d-e3ffffb0eb5f"
#define APP_SECRET        "5853eab2-8c69-497f-8eaa-fcd9e28d02b8-b97279cc-2f7c-4487-bc10-364eb25f49ac"

static const char* switchID[3] = {
  "6a8871c829c6be334299415f",
  "6a887202969af7ec248d2b88",
  "6a88721a969af7ec248d2bb7"
};

/* ===== CALLBACK ===== */
inline bool onPower(const String &deviceId, bool &state) {
  for (int i = 0; i < 3; i++) {
    if (deviceId == switchID[i]) {
      requestExclusive(i, state, switchID);
      return true;
    }
  }
  return true;
}

/* ===== INIT SINRIC ===== */
inline void initSinric() {
  for (int i = 0; i < 3; i++) {
    SinricProSwitch &sw = SinricPro[switchID[i]];
    sw.onPowerState(onPower);
  }

  SinricPro.begin(APP_KEY, APP_SECRET);

  // Sinkron state lokal ke Google Home setelah boot
  for (int i = 0; i < 3; i++) {
    SinricProSwitch &sw = SinricPro[switchID[i]];
    sw.sendPowerStateEvent(getRelayState(i));
  }
}

/* ===== HANDLE ===== */
inline void handleSinric() {
  SinricPro.handle();
}

#endif
