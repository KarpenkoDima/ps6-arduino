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
 * Строка 0: ЧЧ:ММ:СС  TXXXc
 * Строка 1: ДД.ММ.ГГГГ  HXX%
 *
 * KEY1 — установить текущее время/дату.
 * KEY2 — секундомер.
 * KEY3 — настройка будильника.
 * KEY4 — заводской сброс.
 *
 * LED_ALARM горит, пока будильник включён.
 */
AppMode mode_clock() {
    static unsigned long last_display_ms  = 0;
    static unsigned long last_sensor_ms   = 0;
    static float         last_temp        = NAN;
    static float         last_hum         = NAN;
    // Защита от повторного срабатывания в той же минуте.
    static bool          alarm_fired_this_minute = false;

    unsigned long now = millis();

    // DHT11 нужно минимум 2 с между опросами.
    if (now - last_sensor_ms >= 2000UL) {
        last_sensor_ms = now;
        last_temp = read_temperature();
        last_hum  = read_humidity();
    }

    // Обновлять дисплей не чаще 5 раз в секунду — без delay().
    if (now - last_display_ms >= 200UL) {
        last_display_ms = now;

        DateTime t = read_clock();

        // Строка 0: время + температура (ровно 16 символов)
        char line0[17];
        if (!isnan(last_temp)) {
            snprintf(line0, 17, "%02d:%02d:%02d  T%3dC ", t.hour, t.minute, t.second, (int)last_temp);
        } else {
            snprintf(line0, 17, "%02d:%02d:%02d  T---C ", t.hour, t.minute, t.second);
        }
        display_text(0, 0, line0);

        // Строка 1: дата + влажность (ровно 16 символов)
        char line1[17];
        if (!isnan(last_hum)) {
            snprintf(line1, 17, "%02d.%02d.%04d H%3d%%", t.day, t.month, t.year, (int)last_hum);
        } else {
            snprintf(line1, 17, "%02d.%02d.%04d H---%%", t.day, t.month, t.year);
        }
        display_text(1, 0, line1);

        led_set(LED_ALARM, wake_alarm.enabled);

        // Снять блокировку повтора, когда ушли с минуты будильника.
        bool at_alarm_minute = (wake_alarm.enabled
                                && t.hour   == wake_alarm.hour
                                && t.minute == wake_alarm.minute);
        if (!at_alarm_minute) {
            alarm_fired_this_minute = false;
        }

        // Срабатывание ровно на нулевой секунде, не более одного раза за минуту.
        if (at_alarm_minute && t.second == 0 && !alarm_fired_this_minute) {
            alarm_fired_this_minute = true;
            return MODE_RING;
        }
    }

    byte k = pressed_keys;
    pressed_keys = 0;

    if (k & KEY_1_MASK) return MODE_SET_TIME;
    if (k & KEY_2_MASK) return MODE_STOPWATCH;
    if (k & KEY_3_MASK) return MODE_ALARM_SETUP;
    if (k & KEY_4_MASK) return MODE_RESET;

    return MODE_CLOCK;
}
