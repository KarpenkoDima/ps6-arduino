#include "screens.h"
#include "screen_common.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "helpers.h"
#include <Arduino.h>

/**
 * Режим установки текущего времени и даты.
 * Всё состояние хранится в ctx->set_time_s.
 *
 * Поля 0-2 — время (строка 0), поля 3-5 — дата (строка 1).
 * Активное поле отмечается угловыми скобками < >.
 *
 * KEY1 — увеличить.  KEY2 — уменьшить.
 * KEY3 — следующее поле.  KEY4 — записать в RTC и выйти.
 */
AppMode mode_set_time(AppContext* ctx) {
    SetTimeState& s = ctx->set_time_s;

    if (!s.initialized) {
        s.edit        = read_clock();
        s.field       = 0;
        s.initialized = true;
        s.last_update = 0;
        display_clear();
    }

    unsigned long now = millis();
    if (now - s.last_update >= 200UL) {
        s.last_update = now;

        char line0[17], line1[17];

        switch (s.field) {
            case 0:
                snprintf(line0, 17, "<%02d>:%02d:%02d  K4ok", s.edit.hour, s.edit.minute, s.edit.second);
                break;
            case 1:
                snprintf(line0, 17, "%02d:<%02d>:%02d  K4ok", s.edit.hour, s.edit.minute, s.edit.second);
                break;
            case 2:
                snprintf(line0, 17, "%02d:%02d:<%02d>  K4ok", s.edit.hour, s.edit.minute, s.edit.second);
                break;
            default:
                snprintf(line0, 17, "%02d:%02d:%02d   K4ok", s.edit.hour, s.edit.minute, s.edit.second);
                break;
        }
        display_text(0, 0, line0);

        switch (s.field) {
            case 3:
                snprintf(line1, 17, "<%02d>.%02d.%04d    ", s.edit.day, s.edit.month, s.edit.year);
                break;
            case 4:
                snprintf(line1, 17, "%02d.<%02d>.%04d    ", s.edit.day, s.edit.month, s.edit.year);
                break;
            case 5:
                snprintf(line1, 17, "%02d.%02d.<%04d>    ", s.edit.day, s.edit.month, s.edit.year);
                break;
            default:
                snprintf(line1, 17, "%02d.%02d.%04d      ", s.edit.day, s.edit.month, s.edit.year);
                break;
        }
        display_text(1, 0, line1);
    }

    byte k = ctx->pressed_keys;
    ctx->pressed_keys = 0;

    if (k & KEY_1_MASK) {
        switch (s.field) {
            case 0: s.edit.hour   = step_up(s.edit.hour,   24); break;
            case 1: s.edit.minute = step_up(s.edit.minute,  60); break;
            case 2: s.edit.second = step_up(s.edit.second,  60); break;
            case 3: s.edit.day    = (s.edit.day   >= 31)   ? 1    : s.edit.day   + 1; break;
            case 4: s.edit.month  = (s.edit.month >= 12)   ? 1    : s.edit.month + 1; break;
            case 5: s.edit.year   = (s.edit.year  >= 2099) ? 2020 : s.edit.year  + 1; break;
        }
        s.last_update = 0;
    }
    if (k & KEY_2_MASK) {
        switch (s.field) {
            case 0: s.edit.hour   = step_down(s.edit.hour,   24); break;
            case 1: s.edit.minute = step_down(s.edit.minute,  60); break;
            case 2: s.edit.second = step_down(s.edit.second,  60); break;
            case 3: s.edit.day    = (s.edit.day   <= 1)    ? 31   : s.edit.day   - 1; break;
            case 4: s.edit.month  = (s.edit.month <= 1)    ? 12   : s.edit.month - 1; break;
            case 5: s.edit.year   = (s.edit.year  <= 2020) ? 2099 : s.edit.year  - 1; break;
        }
        s.last_update = 0;
    }
    if (k & KEY_3_MASK) {
        s.field = (s.field + 1) % 6;
        s.last_update = 0;
    }
    if (k & KEY_4_MASK) {
        write_clock(s.edit.day, s.edit.month, s.edit.year,
                    s.edit.hour, s.edit.minute, s.edit.second);
        s.initialized = false;
        display_clear();
        return MODE_CLOCK;
    }

    return MODE_SET_TIME;
}
