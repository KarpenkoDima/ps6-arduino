#include "screens.h"
#include "screen_common.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "sensors.h"
#include <Arduino.h>

/**
 * Режим заводского сброса.
 * Состояние (hold_start_ms) хранится в ctx->reset_s — без static-локалей.
 *
 * Нужно удерживать KEY4 в течение RESET_HOLD_MS мс.
 * KEY1 — отмена.
 */
AppMode mode_reset(AppContext* ctx) {
    ResetState& s = ctx->reset_s;

    display_clear();
    display_text(0, 0, "Hold K4 to reset");
    display_text(1, 0, "K1: cancel      ");

    bool key4_down = digitalRead(KEY_PIN_4) == LOW;

    if (key4_down) {
        if (s.hold_start_ms == 0) s.hold_start_ms = millis();
        unsigned long held = millis() - s.hold_start_ms;

        // Прогресс-бар на нижней строке.
        char bar[17];
        unsigned long pct = (held * 16UL) / RESET_HOLD_MS;
        if (pct > 16) pct = 16;
        for (unsigned long i = 0; i < 16; i++) bar[i] = (i < pct) ? '#' : ' ';
        bar[16] = '\0';
        display_text(1, 0, bar);

        if (held >= RESET_HOLD_MS) {
            s.hold_start_ms = 0;

            ctx->wake_alarm.hour    = DEFAULT_WAKE_HOUR;
            ctx->wake_alarm.minute  = DEFAULT_WAKE_MINUTE;
            ctx->wake_alarm.enabled = false;
            alarm_save(ctx);

            write_clock(1, 1, 2026, 0, 0, 0);

            led_set(LED_ALARM, false);
            led_set(LED_RUN,   false);

            display_clear();
            display_text(0, 0, "Reset done!     ");
            delay(1500);
            ctx->pressed_keys = 0;
            return MODE_CLOCK;
        }
    } else {
        s.hold_start_ms = 0;
        byte k = ctx->pressed_keys;
        ctx->pressed_keys = 0;
        if (k & KEY_1_MASK) return MODE_CLOCK;
    }

    return MODE_RESET;
}
