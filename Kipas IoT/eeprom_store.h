#ifndef EEPROM_STORE_H
#define EEPROM_STORE_H

#include <Arduino.h>
#include <EEPROM.h>

/* ===== KONFIG ===== */
#define EEPROM_STORE_SIZE 32   // cukup & aman untuk ESP32

/* ===== INIT (DIPANGGIL SEKALI) ===== */
inline void eepromStoreBegin() {
  static bool initialized = false;
  if (!initialized) {
    EEPROM.begin(EEPROM_STORE_SIZE);
    initialized = true;
  }
}

/* ===== READ BOOL (DEFAULT SAFE) ===== */
inline bool eepromReadBool(uint8_t addr, bool defaultValue = false) {
  uint8_t val = EEPROM.read(addr);
  if (val == 0xFF) return defaultValue;   // flash kosong
  return val;
}

/* ===== WRITE BOOL (FLASH SAFE) ===== */
inline void eepromWriteBool(uint8_t addr, bool value) {
  uint8_t current = EEPROM.read(addr);
  if (current == value) return;           // proteksi write
  EEPROM.write(addr, value);
  EEPROM.commit();
}

#endif
