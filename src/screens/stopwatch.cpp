#include "screens.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "sensors.h"
#include <Arduino.h>

/**
 * Режим секундомера.
 * Всё состояние хранится в ctx->stopwatch_s.
 *
 * KEY1 — выйти к часам (состояние сохраняется).
 * KEY2 — сброс на 00:00.0.
 * KEY3 — старт/пауза.
 * KEY4 — перейти к таймеру обратного отсчёта.
 */
AppMode mode_stopwatch(AppContext* ctx) {
    StopwatchState& s = ctx->stopwatch_s;

    byte k = ctx->pressed_keys;
    ctx->pressed_keys = 0;

    if (k & KEY_3_MASK) {
        if (s.running) {
            s.accumulated_ms += millis() - s.started_at;
            s.running = false;
            led_set(LED_RUN, false);
        } else {
            s.started_at = millis();
            s.running    = true;
            led_set(LED_RUN, true);
        }
    }

    if (k & KEY_2_MASK) {
        s.running        = false;
        s.accumulated_ms = 0;
        s.started_at     = millis();
        led_set(LED_RUN, false);
    }

    if (k & KEY_1_MASK) return MODE_CLOCK;

    if (k & KEY_4_MASK) {
        ctx->timer_s.setup_phase = true;
        return MODE_TIMER;
    }

    unsigned long total_ms = s.accumulated_ms;
    if (s.running) total_ms += millis() - s.started_at;

    unsigned long tenths   = total_ms / 100;
    unsigned long minutes  = tenths / 600;
    unsigned long seconds  = (tenths / 10) % 60;
    unsigned long fraction = tenths % 10;

    if (minutes > 99) { minutes = 99; seconds = 59; fraction = 9; }

    char line[17];
    sprintf(line, "SW %02lu:%02lu.%lu %s",
            minutes, seconds, fraction, s.running ? "RUN" : "PAU");

    display_clear();
    display_text(0, 0, line);
    display_text(1, 0, "K1X K2rs K3go K4");

    delay(80);
    return MODE_STOPWATCH;
}
