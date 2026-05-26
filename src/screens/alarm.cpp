#include "screens.h"
#include "screen_common.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "sensors.h"
#include <Arduino.h>

/**
 * Режим звонка будильника.
 * Включает буззер серией коротких бипов, обе лампочки мигают противофазой.
 *
 * KEY1 — выключить будильник, вернуться в часы.
 * KEY2 — отложить (snooze) на 5 минут.
 * Остальные кнопки игнорируются, чтобы случайным нажатием не выключить.
 */
AppMode mode_ring() {
    leds_alarm_blink(true);

    // Серия из коротких сигналов: 100 мс звук, 250 мс пауза.
    beep(100);
    delay(250);

    byte k = pressed_keys;
    pressed_keys = 0;

    if (k & KEY_1_MASK) {
        leds_alarm_blink(false);
        return MODE_CLOCK;
    }

    if (k & KEY_2_MASK) {
        // Snooze: переводим минуты будильника на +5, при необходимости
        // переносим час, и сохраняем в EEPROM.
        byte new_min = wake_alarm.minute + 5;
        if (new_min >= 60) {
            new_min -= 60;
            wake_alarm.hour = (wake_alarm.hour + 1) % 24;
        }
        wake_alarm.minute = new_min;
        alarm_save();

        leds_alarm_blink(false);
        display_clear();
        display_text(0, 0, "Snooze +5 min");
        delay(1500);
        return MODE_CLOCK;
    }

    return MODE_RING;
}
