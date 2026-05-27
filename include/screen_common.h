#ifndef SCREEN_COMMON_H
#define SCREEN_COMMON_H

#include <Arduino.h>

// Используется в сигнатурах ниже; полное определение — в context.h.
struct AppContext;

// Загрузить настройки будильника из EEPROM → ctx->wake_alarm.
void alarm_load(AppContext* ctx);

// Сохранить ctx->wake_alarm в EEPROM (только при изменении ячейки).
void alarm_save(const AppContext* ctx);

// Короткий звуковой сигнал заданной длительности.
void beep(int ms);

#endif
