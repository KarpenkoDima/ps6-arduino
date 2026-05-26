#include "screen_common.h"
#include "config.h"
#include <Arduino.h>
#include <EEPROM.h>

// Адреса EEPROM для будильника.
// Перед основными байтами хранится magic — простая защита от чтения
// мусора, если EEPROM ещё ни разу не был инициализирован.
#define EEPROM_ALARM_MAGIC_ADDR 0
#define EEPROM_ALARM_HOUR_ADDR  1
#define EEPROM_ALARM_MIN_ADDR   2
#define EEPROM_ALARM_FLAG_ADDR  3
#define EEPROM_ALARM_MAGIC      0x42

// Глобальное состояние будильника, общее для всех экранов.
Alarm wake_alarm = { DEFAULT_WAKE_HOUR, DEFAULT_WAKE_MINUTE, false };
bool timer_setup_phase = true;

/**
 * Загружает будильник из EEPROM. Если magic не совпадает (первая прошивка
 * на чистом Arduino), берёт значения по умолчанию из config.h.
 */
void alarm_load() {
    if (EEPROM.read(EEPROM_ALARM_MAGIC_ADDR) != EEPROM_ALARM_MAGIC) {
        wake_alarm.hour    = DEFAULT_WAKE_HOUR;
        wake_alarm.minute  = DEFAULT_WAKE_MINUTE;
        wake_alarm.enabled = false;
        return;
    }
    byte h = EEPROM.read(EEPROM_ALARM_HOUR_ADDR);
    byte m = EEPROM.read(EEPROM_ALARM_MIN_ADDR);
    byte f = EEPROM.read(EEPROM_ALARM_FLAG_ADDR);
    wake_alarm.hour    = (h <= 23) ? h : DEFAULT_WAKE_HOUR;
    wake_alarm.minute  = (m <= 59) ? m : DEFAULT_WAKE_MINUTE;
    wake_alarm.enabled = (f != 0);
}

/**
 * Сохраняет будильник в EEPROM. Использует EEPROM.update — реальная
 * запись произойдёт только если значение изменилось, что бережёт ресурс
 * чипа EEPROM (≈100 000 циклов записи на ячейку).
 */
void alarm_save() {
    EEPROM.update(EEPROM_ALARM_HOUR_ADDR, wake_alarm.hour);
    EEPROM.update(EEPROM_ALARM_MIN_ADDR,  wake_alarm.minute);
    EEPROM.update(EEPROM_ALARM_FLAG_ADDR, wake_alarm.enabled ? 1 : 0);
    EEPROM.update(EEPROM_ALARM_MAGIC_ADDR, EEPROM_ALARM_MAGIC);
}

/**
 * Короткий звуковой сигнал заданной длительности.
 * Использует встроенную функцию tone(), частота — из config.h.
 */
void beep(int ms) {
    tone(BUZZER_PIN, BUZZER_FREQ, ms);
    delay(ms);
    noTone(BUZZER_PIN);
}
