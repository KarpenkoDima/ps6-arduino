#include "screens.h"
#include "screen_common.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "helpers.h"
#include <Arduino.h>

/**
 * Режим настройки будильника — полноценный state machine без while(true).
 * Всё состояние экрана хранится в ctx->alarm_setup_s.
 * Сброс состояния снаружи: ctx->alarm_setup_s = AlarmSetupState{};
 *
 * KEY1 — увеличить поле.  KEY2 — уменьшить.
 * KEY3 — следующее поле.  KEY4 — сохранить в EEPROM и выйти.
 */
AppMode mode_alarm_setup(AppContext* ctx) {
    AlarmSetupState& s = ctx->alarm_setup_s;

    if (!s.initialized) {
        s.field       = 0;
        s.initialized = true;
        s.last_update = 0;
        display_clear();
    }

    unsigned long now = millis();
    if (now - s.last_update >= 200UL) {
        s.last_update = now;

        char line0[17];
        if (s.field == 0) {
            snprintf(line0, 17, "Wake <%02d>:%02d %s",
                     ctx->wake_alarm.hour, ctx->wake_alarm.minute,
                     ctx->wake_alarm.enabled ? "ON " : "OFF");
        } else if (s.field == 1) {
            snprintf(line0, 17, "Wake %02d:<%02d> %s",
                     ctx->wake_alarm.hour, ctx->wake_alarm.minute,
                     ctx->wake_alarm.enabled ? "ON " : "OFF");
        } else {
            snprintf(line0, 17, "Wake %02d:%02d <%s>",
                     ctx->wake_alarm.hour, ctx->wake_alarm.minute,
                     ctx->wake_alarm.enabled ? "ON " : "OFF");
        }
        display_text(0, 0, line0);
        display_text(1, 0, "K1+ K2- K3> K4ok");
    }

    byte k = ctx->pressed_keys;
    ctx->pressed_keys = 0;

    if (k & KEY_1_MASK) {
        if (s.field == 0) ctx->wake_alarm.hour    = step_up(ctx->wake_alarm.hour,   24);
        if (s.field == 1) ctx->wake_alarm.minute  = step_up(ctx->wake_alarm.minute,  60);
        if (s.field == 2) ctx->wake_alarm.enabled = !ctx->wake_alarm.enabled;
        s.last_update = 0;
    }
    if (k & KEY_2_MASK) {
        if (s.field == 0) ctx->wake_alarm.hour    = step_down(ctx->wake_alarm.hour,   24);
        if (s.field == 1) ctx->wake_alarm.minute  = step_down(ctx->wake_alarm.minute,  60);
        if (s.field == 2) ctx->wake_alarm.enabled = !ctx->wake_alarm.enabled;
        s.last_update = 0;
    }
    if (k & KEY_3_MASK) {
        s.field = (s.field + 1) % 3;
        s.last_update = 0;
    }
    if (k & KEY_4_MASK) {
        alarm_save(ctx);
        s.initialized = false;
        display_clear();
        return MODE_CLOCK;
    }

    return MODE_ALARM_SETUP;
}
