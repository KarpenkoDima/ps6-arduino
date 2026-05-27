#include "screens.h"
#include "screen_common.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "sensors.h"
#include <Arduino.h>

/**
 * Режим таймера обратного отсчёта.
 * Всё состояние хранится в ctx->timer_s.
 *
 * Фаза установки (s.setup_phase == true):
 *   KEY1/KEY2 — больше/меньше минут.  KEY3 — старт.  KEY4 — выход.
 *
 * Фаза работы:
 *   KEY3 — пауза/продолжить.  KEY4 — выход к часам.
 */
AppMode mode_timer(AppContext* ctx) {
    TimerState& s = ctx->timer_s;
    char line[17];

    byte k = ctx->pressed_keys;
    ctx->pressed_keys = 0;

    if (s.setup_phase) {
        led_set(LED_RUN, false);
        display_clear();
        sprintf(line, "Timer set %02d min", s.set_minutes);
        display_text(0, 0, line);
        display_text(1, 0, "K1+ K2- K3go K4X");

        if (k & KEY_1_MASK) s.set_minutes = (s.set_minutes >= 99) ? 99 : s.set_minutes + 1;
        if (k & KEY_2_MASK) s.set_minutes = (s.set_minutes <= 1)  ? 1  : s.set_minutes - 1;
        if (k & KEY_3_MASK) {
            s.remaining_seconds = (long)s.set_minutes * 60L;
            s.counting          = true;
            s.setup_phase       = false;
            s.last_tick_ms      = millis();
            led_set(LED_RUN, true);
        }
        if (k & KEY_4_MASK) return MODE_CLOCK;
        return MODE_TIMER;
    }

    // Фаза обратного отсчёта.
    if (s.counting && s.remaining_seconds > 0) {
        unsigned long ms = millis();
        if (ms - s.last_tick_ms >= 1000) {
            s.last_tick_ms = ms;
            s.remaining_seconds--;
        }
    }

    long mm = s.remaining_seconds / 60;
    long ss = s.remaining_seconds % 60;

    display_clear();
    sprintf(line, "Timer %02ld:%02ld %s", mm, ss, s.counting ? "RUN" : "PAU");
    display_text(0, 0, line);
    display_text(1, 0, s.counting ? "K3pause   K4exit" : "K3resume  K4exit");

    if (k & KEY_3_MASK) {
        s.counting = !s.counting;
        led_set(LED_RUN, s.counting);
        if (s.counting) s.last_tick_ms = millis();
    }
    if (k & KEY_4_MASK) {
        s.counting          = false;
        s.setup_phase       = true;
        s.set_minutes       = 1;
        s.remaining_seconds = 0;
        led_set(LED_RUN, false);
        return MODE_CLOCK;
    }

    if (s.remaining_seconds <= 0 && !s.counting) {
        display_clear();
        display_text(0, 0, "  TIME IS UP!   ");
        for (byte i = 0; i < 3; i++) { beep(120); delay(100); }
        while (true) {
            scan_keys(ctx);
            byte kk = ctx->pressed_keys;
            ctx->pressed_keys = 0;
            if (kk) break;
            delay(20);
        }
        s.setup_phase       = true;
        s.set_minutes       = 1;
        s.remaining_seconds = 0;
        led_set(LED_RUN, false);
        return MODE_CLOCK;
    }

    if (s.remaining_seconds <= 0 && s.counting) {
        s.counting = false;
        led_set(LED_RUN, false);
    }

    return MODE_TIMER;
}
