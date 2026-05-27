#include "screens.h"
#include "screen_common.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "helpers.h"
#include <Arduino.h>

/**
 * Режим настройки будильника — полноценный state machine без while(true).
 *
 * Активное поле помечается угловыми скобками < >.
 *
 * KEY1 — увеличить выбранное поле (час / минута / ON↔OFF).
 * KEY2 — уменьшить.
 * KEY3 — следующее поле (час → минута → ON/OFF → снова час).
 * KEY4 — сохранить в EEPROM и вернуться к часам.
 */
AppMode mode_alarm_setup() {
    static byte          field       = 0;
    static bool          initialized = false;
    static unsigned long last_update = 0;

    if (!initialized) {
        field       = 0;
        initialized = true;
        last_update = 0;  // форсировать немедленную первую прорисовку
        display_clear();
    }

    // Обновлять дисплей не чаще 5 раз в секунду.
    unsigned long now = millis();
    if (now - last_update >= 200UL) {
        last_update = now;

        char line0[17];
        if (field == 0) {
            snprintf(line0, 17, "Wake <%02d>:%02d %s",
                     wake_alarm.hour, wake_alarm.minute,
                     wake_alarm.enabled ? "ON " : "OFF");
        } else if (field == 1) {
            snprintf(line0, 17, "Wake %02d:<%02d> %s",
                     wake_alarm.hour, wake_alarm.minute,
                     wake_alarm.enabled ? "ON " : "OFF");
        } else {
            snprintf(line0, 17, "Wake %02d:%02d <%s>",
                     wake_alarm.hour, wake_alarm.minute,
                     wake_alarm.enabled ? "ON " : "OFF");
        }
        display_text(0, 0, line0);
        display_text(1, 0, "K1+ K2- K3> K4ok");
    }

    byte k = pressed_keys;
    pressed_keys = 0;

    if (k & KEY_1_MASK) {
        if (field == 0) wake_alarm.hour    = step_up(wake_alarm.hour,   24);
        if (field == 1) wake_alarm.minute  = step_up(wake_alarm.minute,  60);
        if (field == 2) wake_alarm.enabled = !wake_alarm.enabled;
        last_update = 0;  // форсировать немедленное обновление дисплея
    }
    if (k & KEY_2_MASK) {
        if (field == 0) wake_alarm.hour    = step_down(wake_alarm.hour,   24);
        if (field == 1) wake_alarm.minute  = step_down(wake_alarm.minute,  60);
        if (field == 2) wake_alarm.enabled = !wake_alarm.enabled;
        last_update = 0;
    }
    if (k & KEY_3_MASK) {
        field = (field + 1) % 3;
        last_update = 0;
    }
    if (k & KEY_4_MASK) {
        alarm_save();
        initialized = false;
        display_clear();
        return MODE_CLOCK;
    }

    return MODE_ALARM_SETUP;
}
