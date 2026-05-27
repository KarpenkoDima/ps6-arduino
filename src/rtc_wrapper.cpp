#include "rtc_wrapper.h"
#include "config.h"
#include <EEPROM.h>

// Адрес EEPROM для хранения хэша стартового времени.
// Адреса 0-3 заняты будильником (screens/init.cpp).
#define EEPROM_BOOT_HASH_ADDR 16

// Хэш параметров RTC_BOOT_* из config.h.
// Если он изменился (ты обновил config.h), при следующей прошивке
// время в RTC автоматически перепишется.
static uint32_t make_boot_hash() {
    uint32_t h = 0;
    h = h * 31 + RTC_BOOT_YEAR;
    h = h * 31 + RTC_BOOT_MONTH;
    h = h * 31 + RTC_BOOT_DAY;
    h = h * 31 + RTC_BOOT_HOUR;
    h = h * 31 + RTC_BOOT_MINUTE;
    h = h * 31 + RTC_BOOT_SECOND;
    return h;
}

// ════════════════════════════════════════════════════════════════════════════
// DS1307 — I2C RTC (используется в среде [env:wokwi])
// ════════════════════════════════════════════════════════════════════════════
#ifdef USE_DS1307

#include <Wire.h>

#define DS1307_ADDR 0x68

static uint8_t bcd2dec(uint8_t b) { return (b >> 4) * 10 + (b & 0x0F); }
static uint8_t dec2bcd(uint8_t d) { return ((d / 10) << 4) | (d % 10); }

void chrono_init() {
    Wire.begin();

    // Читаем регистр секунд: бит 7 (CH) = 1 означает, что часы стоят.
    Wire.beginTransmission(DS1307_ADDR);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)DS1307_ADDR, (uint8_t)1);
    uint8_t secs_reg = Wire.available() ? Wire.read() : 0x80;
    bool clock_halted = (secs_reg & 0x80) != 0;

    uint32_t expected = make_boot_hash();
    uint32_t stored   = 0;
    EEPROM.get(EEPROM_BOOT_HASH_ADDR, stored);

    if (clock_halted || stored != expected) {
        Wire.beginTransmission(DS1307_ADDR);
        Wire.write(0x00);
        Wire.write(dec2bcd(RTC_BOOT_SECOND));          // CH=0 → запускаем часы
        Wire.write(dec2bcd(RTC_BOOT_MINUTE));
        Wire.write(dec2bcd(RTC_BOOT_HOUR));
        Wire.write(0x01);                               // день недели (1=вс)
        Wire.write(dec2bcd(RTC_BOOT_DAY));
        Wire.write(dec2bcd(RTC_BOOT_MONTH));
        Wire.write(dec2bcd((uint8_t)(RTC_BOOT_YEAR - 2000)));
        Wire.endTransmission();
        EEPROM.put(EEPROM_BOOT_HASH_ADDR, expected);
    }
}

DateTime read_clock() {
    Wire.beginTransmission(DS1307_ADDR);
    Wire.write(0x00);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)DS1307_ADDR, (uint8_t)7);

    DateTime out = {};
    if (Wire.available() >= 7) {
        out.second = bcd2dec(Wire.read() & 0x7F);  // маска бита CH
        out.minute = bcd2dec(Wire.read());
        out.hour   = bcd2dec(Wire.read() & 0x3F);  // маска бита 12/24h
        Wire.read();                                 // пропуск дня недели
        out.day    = bcd2dec(Wire.read());
        out.month  = bcd2dec(Wire.read());
        out.year   = 2000 + (int)bcd2dec(Wire.read());
    }
    return out;
}

void write_clock(byte day, byte month, int year,
                 byte hour, byte minute, byte second) {
    Wire.beginTransmission(DS1307_ADDR);
    Wire.write(0x00);
    Wire.write(dec2bcd(second));
    Wire.write(dec2bcd(minute));
    Wire.write(dec2bcd(hour));
    Wire.write(0x01);
    Wire.write(dec2bcd(day));
    Wire.write(dec2bcd(month));
    Wire.write(dec2bcd((uint8_t)(year - 2000)));
    Wire.endTransmission();
}

// ════════════════════════════════════════════════════════════════════════════
// DS1302 — 3-wire RTC (используется в среде [env:uno], реальное железо)
// ════════════════════════════════════════════════════════════════════════════
#else

#include <DS1302.h>

static DS1302 chip(RTC_RST_PIN, RTC_DAT_PIN, RTC_CLK_PIN);

static bool rtc_value_lost(const Time& t) {
    return t.yr < RTC_BOOT_YEAR - 1 || t.yr > RTC_BOOT_YEAR + 20;
}

void chrono_init() {
    chip.writeProtect(false);
    chip.halt(false);

    uint32_t expected = make_boot_hash();
    uint32_t stored   = 0;
    EEPROM.get(EEPROM_BOOT_HASH_ADDR, stored);

    Time current = chip.time();
    if (stored != expected || rtc_value_lost(current)) {
        Time fresh(RTC_BOOT_YEAR, RTC_BOOT_MONTH, RTC_BOOT_DAY,
                   RTC_BOOT_HOUR, RTC_BOOT_MINUTE, RTC_BOOT_SECOND,
                   Time::kSunday);
        chip.time(fresh);
        EEPROM.put(EEPROM_BOOT_HASH_ADDR, expected);
    }
}

DateTime read_clock() {
    Time t = chip.time();
    DateTime out;
    out.day    = t.date;
    out.month  = t.mon;
    out.year   = t.yr;
    out.hour   = t.hr;
    out.minute = t.min;
    out.second = t.sec;
    return out;
}

void write_clock(byte day, byte month, int year,
                 byte hour, byte minute, byte second) {
    Time t(year, month, day, hour, minute, second, Time::kSunday);
    chip.time(t);
}

#endif
