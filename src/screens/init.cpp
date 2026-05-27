#include "screen_common.h"
#include "context.h"
#include "config.h"
#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_ALARM_MAGIC_ADDR 0
#define EEPROM_ALARM_HOUR_ADDR  1
#define EEPROM_ALARM_MIN_ADDR   2
#define EEPROM_ALARM_FLAG_ADDR  3
#define EEPROM_ALARM_MAGIC      0x42

void alarm_load(AppContext* ctx) {
    if (EEPROM.read(EEPROM_ALARM_MAGIC_ADDR) != EEPROM_ALARM_MAGIC) {
        ctx->wake_alarm.hour    = DEFAULT_WAKE_HOUR;
        ctx->wake_alarm.minute  = DEFAULT_WAKE_MINUTE;
        ctx->wake_alarm.enabled = false;
        return;
    }
    byte h = EEPROM.read(EEPROM_ALARM_HOUR_ADDR);
    byte m = EEPROM.read(EEPROM_ALARM_MIN_ADDR);
    byte f = EEPROM.read(EEPROM_ALARM_FLAG_ADDR);
    ctx->wake_alarm.hour    = (h <= 23) ? h : DEFAULT_WAKE_HOUR;
    ctx->wake_alarm.minute  = (m <= 59) ? m : DEFAULT_WAKE_MINUTE;
    ctx->wake_alarm.enabled = (f != 0);
}

void alarm_save(const AppContext* ctx) {
    EEPROM.update(EEPROM_ALARM_HOUR_ADDR,  ctx->wake_alarm.hour);
    EEPROM.update(EEPROM_ALARM_MIN_ADDR,   ctx->wake_alarm.minute);
    EEPROM.update(EEPROM_ALARM_FLAG_ADDR,  ctx->wake_alarm.enabled ? 1 : 0);
    EEPROM.update(EEPROM_ALARM_MAGIC_ADDR, EEPROM_ALARM_MAGIC);
}

void beep(int ms) {
    tone(BUZZER_PIN, BUZZER_FREQ, ms);
    delay(ms);
    noTone(BUZZER_PIN);
}
