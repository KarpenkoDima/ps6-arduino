#include "screens.h"
#include "screen_common.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "sensors.h"
#include "helpers.h"
#include <Arduino.h>

/**
 * Подэкран настройки будильника.
 * Активное поле подсвечивается треугольными скобками, чтобы было видно,
 * что именно сейчас редактируется.
 *
 * KEY1 — увеличить выбранное поле (час, минуту или ON↔OFF).
 * KEY2 — уменьшить.
 * KEY3 — следующее поле (час → минута → ON/OFF → опять час).
 * KEY4 — сохранить и выйти.
 */
static void run_alarm_setup() {
    byte field = 0;     // 0 — часы, 1 — минуты, 2 — флаг включения.
    char line[17];

    while (true) {
        display_clear();

        if (field == 0) {
            sprintf(line, "Wake <%02d>:%02d %s",
                    wake_alarm.hour, wake_alarm.minute,
                    wake_alarm.enabled ? "ON " : "OFF");
        } else if (field == 1) {
            sprintf(line, "Wake %02d:<%02d> %s",
                    wake_alarm.hour, wake_alarm.minute,
                    wake_alarm.enabled ? "ON " : "OFF");
        } else {
            sprintf(line, "Wake %02d:%02d <%s>",
                    wake_alarm.hour, wake_alarm.minute,
                    wake_alarm.enabled ? "ON " : "OFF");
        }
        display_text(0, 0, line);

        delay(180);
        scan_keys();
        byte k = pressed_keys;
        pressed_keys = 0;

        if (k & KEY_1_MASK) {
            if (field == 0) wake_alarm.hour    = step_up(wake_alarm.hour, 24);
            if (field == 1) wake_alarm.minute  = step_up(wake_alarm.minute, 60);
            if (field == 2) wake_alarm.enabled = !wake_alarm.enabled;
        }
        if (k & KEY_2_MASK) {
            if (field == 0) wake_alarm.hour    = step_down(wake_alarm.hour, 24);
            if (field == 1) wake_alarm.minute  = step_down(wake_alarm.minute, 60);
            if (field == 2) wake_alarm.enabled = !wake_alarm.enabled;
        }
        if (k & KEY_3_MASK) {
            field = (field + 1) % 3;
        }
        if (k & KEY_4_MASK) {
            alarm_save();
            return;
        }
    }
}

/**
 * Главный режим — отображение часов и даты.
 *
 * KEY1 — открыть секундомер.
 * KEY2 — открыть таймер.
 * KEY3 — открыть настройку будильника.
 * KEY4 — переход в меню сброса (factory reset).
 *
 * Лампочка LED_ALARM горит всегда, когда будильник активирован,
 * что соответствует требованию задания: пользователь видит, что
 * будильник включён, даже не глядя на экран.
 */
AppMode mode_clock() {
    DateTime t = read_clock();
    char line[17];

    display_clear();
    sprintf(line, "%02d:%02d:%02d", t.hour, t.minute, t.second);
    display_text(0, 4, line);
    sprintf(line, "%02d.%02d.%04d", t.day, t.month, t.year);
    display_text(1, 3, line);

    led_set(LED_ALARM, wake_alarm.enabled);

    // Срабатывание будильника происходит ровно на нулевой секунде
    // подходящей минуты — иначе режим звонка вызвался бы много раз.
    if (wake_alarm.enabled
        && t.hour == wake_alarm.hour
        && t.minute == wake_alarm.minute
        && t.second == 0) {
        return MODE_RING;
    }

    delay(200);

    byte k = pressed_keys;
    pressed_keys = 0;

    if (k & KEY_1_MASK) return MODE_STOPWATCH;
    if (k & KEY_2_MASK) {
        timer_setup_phase = true;
        return MODE_TIMER;
    }
    if (k & KEY_3_MASK) {
        run_alarm_setup();
        return MODE_CLOCK;
    }
    if (k & KEY_4_MASK) return MODE_RESET;

    return MODE_CLOCK;
}
