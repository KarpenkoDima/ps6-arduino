#include "screens.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "helpers.h"
#include <Arduino.h>

/**
 * Режим установки текущего времени и даты.
 *
 * Строки дисплея показывают редактируемые значения; активное поле
 * помечается угловыми скобками < >.  Поля 0-2 — время (строка 0),
 * поля 3-5 — дата (строка 1).
 *
 * KEY1 — увеличить выбранное поле.
 * KEY2 — уменьшить выбранное поле.
 * KEY3 — следующее поле (ЧЧ → ММ → СС → ДД → ММ → ГГГГ → снова ЧЧ).
 * KEY4 — записать в RTC и вернуться к часам.
 */
AppMode mode_set_time() {
    // field: 0=hour 1=minute 2=second 3=day 4=month 5=year
    static byte     field       = 0;
    static bool     initialized = false;
    static DateTime edit;
    static unsigned long last_update = 0;

    if (!initialized) {
        edit        = read_clock();
        field       = 0;
        initialized = true;
        display_clear();
    }

    unsigned long now = millis();
    if (now - last_update >= 200UL) {
        last_update = now;

        char line0[17], line1[17];

        // Строка 0 — время с подсветкой активного поля (0-2).
        switch (field) {
            case 0:
                snprintf(line0, 17, "<%02d>:%02d:%02d  K4ok", edit.hour, edit.minute, edit.second);
                break;
            case 1:
                snprintf(line0, 17, "%02d:<%02d>:%02d  K4ok", edit.hour, edit.minute, edit.second);
                break;
            case 2:
                snprintf(line0, 17, "%02d:%02d:<%02d>  K4ok", edit.hour, edit.minute, edit.second);
                break;
            default:
                snprintf(line0, 17, "%02d:%02d:%02d   K4ok", edit.hour, edit.minute, edit.second);
                break;
        }
        display_text(0, 0, line0);

        // Строка 1 — дата с подсветкой активного поля (3-5).
        switch (field) {
            case 3:
                snprintf(line1, 17, "<%02d>.%02d.%04d    ", edit.day, edit.month, edit.year);
                break;
            case 4:
                snprintf(line1, 17, "%02d.<%02d>.%04d    ", edit.day, edit.month, edit.year);
                break;
            case 5:
                snprintf(line1, 17, "%02d.%02d.<%04d>    ", edit.day, edit.month, edit.year);
                break;
            default:
                snprintf(line1, 17, "%02d.%02d.%04d      ", edit.day, edit.month, edit.year);
                break;
        }
        display_text(1, 0, line1);
    }

    byte k = pressed_keys;
    pressed_keys = 0;

    if (k & KEY_1_MASK) {
        switch (field) {
            case 0: edit.hour   = step_up(edit.hour,   24); break;
            case 1: edit.minute = step_up(edit.minute,  60); break;
            case 2: edit.second = step_up(edit.second,  60); break;
            case 3: edit.day    = (edit.day   >= 31) ? 1  : edit.day   + 1; break;
            case 4: edit.month  = (edit.month >= 12) ? 1  : edit.month + 1; break;
            case 5: edit.year   = (edit.year  >= 2099) ? 2020 : edit.year + 1; break;
        }
        last_update = 0;
    }
    if (k & KEY_2_MASK) {
        switch (field) {
            case 0: edit.hour   = step_down(edit.hour,   24); break;
            case 1: edit.minute = step_down(edit.minute,  60); break;
            case 2: edit.second = step_down(edit.second,  60); break;
            case 3: edit.day    = (edit.day   <= 1)  ? 31   : edit.day   - 1; break;
            case 4: edit.month  = (edit.month <= 1)  ? 12   : edit.month - 1; break;
            case 5: edit.year   = (edit.year  <= 2020) ? 2099 : edit.year - 1; break;
        }
        last_update = 0;
    }
    if (k & KEY_3_MASK) {
        field = (field + 1) % 6;
        last_update = 0;
    }
    if (k & KEY_4_MASK) {
        write_clock(edit.day, edit.month, edit.year,
                    edit.hour, edit.minute, edit.second);
        initialized = false;
        display_clear();
        return MODE_CLOCK;
    }

    return MODE_SET_TIME;
}
