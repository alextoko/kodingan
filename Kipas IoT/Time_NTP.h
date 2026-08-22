#ifndef TIME_NTP_H
#define TIME_NTP_H

#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

/* ================== KONFIG WAKTU ================== */
// Zona waktu Indonesia WIB (UTC+7)
#define GMT_OFFSET_SEC     (7 * 3600)
#define DAYLIGHT_OFFSET_SEC 0

// Server NTP (pakai beberapa untuk fallback)
static const char* ntpServer1 = "pool.ntp.org";
static const char* ntpServer2 = "time.nist.gov";

/* ================== STATE ================== */
static bool timeSynced = false;

/* ================== INIT NTP ================== */
inline void initNTPTime() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[TIME] WiFi not connected, skip NTP");
    return;
  }

  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC,
             ntpServer1, ntpServer2);

  Serial.println("[TIME] Syncing time...");

  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 5000)) {
    timeSynced = true;
    Serial.println("[TIME] Time synced");
  } else {
    Serial.println("[TIME] NTP sync failed");
  }
}

/* ================== CEK STATUS ================== */
inline bool isTimeSynced() {
  return timeSynced;
}

/* ================== AMBIL JAM STRING ================== */
inline String getTimeString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "--:--:--";

  char buf[9];
  strftime(buf, sizeof(buf), "%H:%M:%S", &timeinfo);
  return String(buf);
}

/* ================== AMBIL TANGGAL STRING ================== */
inline String getDateString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "--/--/----";

  char buf[11];
  strftime(buf, sizeof(buf), "%d/%m/%Y", &timeinfo);
  return String(buf);
}

/* ================== JAM, MENIT, DETIK ================== */
inline int getHour() {
  struct tm t;
  if (!getLocalTime(&t)) return -1;
  return t.tm_hour;
}

inline int getMinute() {
  struct tm t;
  if (!getLocalTime(&t)) return -1;
  return t.tm_min;
}

inline int getSecond() {
  struct tm t;
  if (!getLocalTime(&t)) return -1;
  return t.tm_sec;
}

#endif
