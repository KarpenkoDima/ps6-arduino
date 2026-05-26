#include "rtc_wrapper.h"
#include "config.h"
#include <DS1302.h>
#include <EEPROM.h>

// Микросхема DS1302. Порядок аргументов библиотеки msparks:
// CE (RST), IO (DAT), SCLK (CLK).
static DS1302 chip(RTC_RST_PIN, RTC_DAT_PIN, RTC_CLK_PIN);

// Адрес EEPROM, куда сохраняем хэш стартового времени.
// Адреса 0..3 заняты будильником в screens/init.cpp.
#define EEPROM_BOOT_HASH_ADDR 16

// Считает простой хэш стартового времени из config.h. Если ты меняешь
// в config.h хоть один параметр RTC_BOOT_* и заливаешь — хэш меняется,
// и тогда chrono_init() запишет новое время в RTC.
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

// Считает время в RTC «потерянным», если возвращаемый год явно
// неадекватный — это бывает после полного отключения питания и
// разряженной батарейки на модуле RTC.
static bool rtc_value_lost(const Time &t) {
    return t.yr < RTC_BOOT_YEAR - 1 || t.yr > RTC_BOOT_YEAR + 20;
}

void chrono_init() {
    chip.writeProtect(false);
    chip.halt(false);

    uint32_t expected = make_boot_hash();
    uint32_t stored = 0;
    EEPROM.get(EEPROM_BOOT_HASH_ADDR, stored);

    Time current = chip.time();
    bool need_write = (stored != expected) || rtc_value_lost(current);

    if (need_write) {
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
