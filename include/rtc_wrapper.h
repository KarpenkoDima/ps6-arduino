#ifndef RTC_WRAPPER_H
#define RTC_WRAPPER_H

#include <Arduino.h>

// Дата + время в одной структуре.
struct DateTime {
    byte day;
    byte month;
    int  year;
    byte hour;
    byte minute;
    byte second;
};

// Запускает RTC DS1302 и при необходимости прошивает стартовое время.
// Время записывается в чип только если значения RTC_BOOT_* в config.h
// поменялись с прошлой прошивки или если RTC явно потерял время
// (например, села батарейка на модуле).
void chrono_init();

// Текущая дата и время одним вызовом.
DateTime read_clock();

// Перезаписать дату и время в RTC.
void write_clock(byte day, byte month, int year,
                 byte hour, byte minute, byte second);

#endif
