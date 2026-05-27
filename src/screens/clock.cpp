#include "screens.h"
#include "screen_common.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "sensors.h"
#include <Arduino.h>
#include <math.h>

/**
 * Главный режим — отображение часов, даты, температуры и влажности.
 *
 * Строка 0: ЧЧ:ММ:СС  T ХХC  (время + температура)
 * Строка 1: ДД.ММ.ГГГГ H ХХ%  (дата + влажность)
 *
 * KEY1 — установить текущее время/дату.
 * KEY2 — секундомер.
 * KEY3 — настройка будильника.
 * KEY4 — заводской сброс.
 */
AppMode mode_clock(AppContext* ctx) {
    ClockState& s = ctx->clock_s;
    unsigned long now = millis();

    if (now - s.last_sensor_ms >= 2000UL) {
        s.last_sensor_ms = now;
        s.last_temp = read_temperature();
        s.last_hum  = read_humidity();
    }

    if (now - s.last_display_ms >= 200UL) {
        s.last_display_ms = now;

        DateTime t = read_clock();

        // Строка 0: ровно 16 символов — время + температура
        char line0[17];
        if (!isnan(s.last_temp)) {
            snprintf(line0, 17, "%02d:%02d:%02d  T%3dC ", t.hour, t.minute, t.second, (int)s.last_temp);
        } else {
            snprintf(line0, 17, "%02d:%02d:%02d  T---C ", t.hour, t.minute, t.second);
        }
        display_text(0, 0, line0);

        // Строка 1: ровно 16 символов — дата + влажность
        char line1[17];
        if (!isnan(s.last_hum)) {
            snprintf(line1, 17, "%02d.%02d.%04d H%3d%%", t.day, t.month, t.year, (int)s.last_hum);
        } else {
            snprintf(line1, 17, "%02d.%02d.%04d H---%%", t.day, t.month, t.year);
        }
        display_text(1, 0, line1);

        led_set(LED_ALARM, ctx->wake_alarm.enabled);

        bool at_alarm_minute = (ctx->wake_alarm.enabled
                                && t.hour   == ctx->wake_alarm.hour
                                && t.minute == ctx->wake_alarm.minute);
        if (!at_alarm_minute) {
            s.alarm_fired_this_minute = false;
        }

        if (at_alarm_minute && t.second == 0 && !s.alarm_fired_this_minute) {
            s.alarm_fired_this_minute = true;
            return MODE_RING;
        }
    }

    byte k = ctx->pressed_keys;
    ctx->pressed_keys = 0;

    if (k & KEY_1_MASK) return MODE_SET_TIME;
    if (k & KEY_2_MASK) return MODE_STOPWATCH;
    if (k & KEY_3_MASK) return MODE_ALARM_SETUP;
    if (k & KEY_4_MASK) return MODE_RESET;

    return MODE_CLOCK;
}
