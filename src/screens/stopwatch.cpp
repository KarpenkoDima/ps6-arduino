#include "screens.h"
#include "screen_common.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "sensors.h"
#include <Arduino.h>

// Внутреннее состояние секундомера. Все значения в миллисекундах.
static bool running = false;
static unsigned long started_at = 0;
static unsigned long accumulated_ms = 0;

/**
 * Режим секундомера.
 *
 * KEY1 — выйти к часам (без сброса, состояние сохранится).
 * KEY2 — сброс на 00:00.0.
 * KEY3 — старт/пауза.
 * KEY4 — перейти к таймеру обратного отсчёта.
 *
 * Точность отображения — 0.1 секунды.
 */
AppMode mode_stopwatch() {
    byte k = pressed_keys;
    pressed_keys = 0;

    if (k & KEY_3_MASK) {
        if (running) {
            accumulated_ms += millis() - started_at;
            running = false;
            led_set(LED_RUN, false);
        } else {
            started_at = millis();
            running = true;
            led_set(LED_RUN, true);
        }
    }

    if (k & KEY_2_MASK) {
        running = false;
        accumulated_ms = 0;
        started_at = millis();
        led_set(LED_RUN, false);
    }

    if (k & KEY_1_MASK) {
        // Не обнуляем — пользователь вернётся и увидит то же значение.
        return MODE_CLOCK;
    }
    if (k & KEY_4_MASK) {
        // Переход к таймеру обратного отсчёта.
        timer_setup_phase = true;
        return MODE_TIMER;
    }

    unsigned long total_ms = accumulated_ms;
    if (running) total_ms += millis() - started_at;

    unsigned long tenths = total_ms / 100;
    unsigned long minutes = tenths / 600;
    unsigned long seconds = (tenths / 10) % 60;
    unsigned long fraction = tenths % 10;

    if (minutes > 99) {
        minutes = 99;
        seconds = 59;
        fraction = 9;
    }

    char line[17];
    sprintf(line, "SW %02lu:%02lu.%lu %s",
            minutes, seconds, fraction, running ? "RUN" : "PAU");

    display_clear();
    display_text(0, 0, line);
    display_text(1, 0, "K1X K2rs K3go K4");

    delay(80);
    return MODE_STOPWATCH;
}
