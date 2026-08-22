#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>
#include "Time_NTP.h"
#include "relay_ctrl.h"
#include "sinric_ctrl.h"
#include "SinricProSwitch.h"

static int lastOffDay = -1;

inline void handleAutoOffAtMidnight(const char* switchID[]) {

  struct tm t;
  if (!getLocalTimeSafe(&t)) return;

  if (t.tm_mday == lastOffDay) return;

  if (t.tm_hour == 0 && t.tm_min == 0) {
    Serial.println("[SCHEDULER] Auto OFF all fans");

    for (int i = 0; i < 3; i++) {
      setRelay(i, false);
    }

    for (int i = 0; i < 3; i++) {
      SinricProSwitch &sw = SinricPro[switchID[i]];
      sw.sendPowerStateEvent(false);
    }

    lastOffDay = t.tm_mday;
  }
}

#endif
