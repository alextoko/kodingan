#ifndef TIME_NTP_H
#define TIME_NTP_H

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

#define GMT_OFFSET_SEC (7 * 3600)

/* ===== INIT NTP ===== */
inline void initNTP() {
  if (WiFi.status() != WL_CONNECTED) return;
  configTime(GMT_OFFSET_SEC, 0, "pool.ntp.org", "time.nist.gov");
}

/* ===== WRAPPER AMAN ===== */
inline bool getLocalTimeSafe(struct tm* t) {
  return getLocalTime(t, 1000); // timeout 1 detik
}

/* ===== HELPER STRING (OPSIONAL) ===== */
inline String getTimeString() {
  struct tm t;
  if (!getLocalTimeSafe(&t)) return "--:--:--";
  char buf[9];
  strftime(buf, sizeof(buf), "%H:%M:%S", &t);
  return String(buf);
}

#endif
