#include "screens.h"
#include "screen_common.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "sensors.h"
#include <Arduino.h>

/**
 * Режим «factory reset».
 * Чтобы сбросить настройки к заводским, нужно удерживать KEY4
 * на протяжении RESET_HOLD_MS миллисекунд (по умолчанию 3 секунды).
 * Это защита от случайного нажатия.
 *
 * После успешного сброса будильник ставится на 07:00 OFF, время RTC
 * сбрасывается на 01.01.2026 00:00:00.
 */
AppMode mode_reset() {
    static unsigned long hold_start_ms = 0;

    display_clear();
    display_text(0, 0, "Hold K4 to reset");
    display_text(1, 0, "K1: cancel");

    bool key4_down = digitalRead(KEY_PIN_4) == LOW;

    if (key4_down) {
        if (hold_start_ms == 0) hold_start_ms = millis();
        unsigned long held = millis() - hold_start_ms;

        // Прогресс-бар на нижней строке: показывает, сколько ещё держать.
        char line[17];
        unsigned long pct = (held * 16UL) / RESET_HOLD_MS;
        if (pct > 16) pct = 16;
        for (unsigned long i = 0; i < 16; i++) line[i] = (i < pct) ? '#' : ' ';
        line[16] = '\0';
        display_text(1, 0, line);

        if (held >= RESET_HOLD_MS) {
            hold_start_ms = 0;

            wake_alarm.hour    = DEFAULT_WAKE_HOUR;
            wake_alarm.minute  = DEFAULT_WAKE_MINUTE;
            wake_alarm.enabled = false;
            alarm_save();

            write_clock(1, 1, 2026, 0, 0, 0);

            led_set(LED_ALARM, false);
            led_set(LED_RUN,   false);

            display_clear();
            display_text(0, 0, "Reset done!");
            delay(1500);
            pressed_keys = 0;
            return MODE_CLOCK;
        }
    } else {
        hold_start_ms = 0;
        // Любое отжатие KEY4 — выходим, если KEY1 нажат.
        byte k = pressed_keys;
        pressed_keys = 0;
        if (k & KEY_1_MASK) return MODE_CLOCK;
    }

    return MODE_RESET;
}
