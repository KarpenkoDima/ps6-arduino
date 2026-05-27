#include "screens.h"
#include "screen_common.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "sensors.h"
#include <Arduino.h>

/**
 * Режим звонка будильника.
 *
 * KEY1 — выключить будильник, вернуться к часам.
 * KEY2 — отложить (snooze) на 5 минут.
 */
AppMode mode_ring(AppContext* ctx) {
    leds_alarm_blink(true);

    beep(100);
    delay(250);

    byte k = ctx->pressed_keys;
    ctx->pressed_keys = 0;

    if (k & KEY_1_MASK) {
        leds_alarm_blink(false);
        return MODE_CLOCK;
    }

    if (k & KEY_2_MASK) {
        byte new_min = ctx->wake_alarm.minute + 5;
        if (new_min >= 60) {
            new_min -= 60;
            ctx->wake_alarm.hour = (ctx->wake_alarm.hour + 1) % 24;
        }
        ctx->wake_alarm.minute = new_min;
        alarm_save(ctx);

        leds_alarm_blink(false);
        display_clear();
        display_text(0, 0, "Snooze +5 min   ");
        delay(1500);
        return MODE_CLOCK;
    }

    return MODE_RING;
}
