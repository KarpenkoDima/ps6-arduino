#ifndef SCREEN_COMMON_H
#define SCREEN_COMMON_H

#include <Arduino.h>

// Настройки будильника, которые видны всем экранам.
struct Alarm {
    byte hour;
    byte minute;
    bool enabled;
};

extern Alarm wake_alarm;

// Состояние таймера, которое нужно сохранять между переключениями
// «настройка таймера ↔ обратный отсчёт».
extern bool timer_setup_phase;

// Загружает/сохраняет будильник в EEPROM с проверкой magic-байта.
void alarm_load();
void alarm_save();

// Короткий «бип» через стандартный tone().
void beep(int ms);

#endif
