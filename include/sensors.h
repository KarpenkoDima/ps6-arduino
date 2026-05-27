#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

// ===== Управление светодиодами =====
// LED_ALARM: горит, когда будильник включён.
// LED_RUN:   горит, когда работает таймер или секундомер.
// Во время сработавшего будильника обе лампочки мигают противофазой.
enum LedId {
    LED_ALARM,
    LED_RUN
};

void leds_init();
void led_set(LedId id, bool on);
void leds_alarm_blink(bool active);
void leds_tick();

// ===== Датчик окружающей среды (DHT11) =====
// Инициализировать DHT11. Вызывать из setup().
void sensors_env_init();

// Вернуть температуру в °C. Возвращает NAN при ошибке чтения.
float read_temperature();

// Вернуть относительную влажность в %. Возвращает NAN при ошибке чтения.
float read_humidity();

#endif
