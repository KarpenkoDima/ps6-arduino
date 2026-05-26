#include "screens.h"
#include "screen_common.h"
#include "context.h"
#include "config.h"
#include "lcd_wrapper.h"
#include "sensors.h"
#include "helpers.h"
#include <Arduino.h>

// Внутреннее состояние таймера.
static int set_minutes = 1;       // выбранное пользователем число минут
static long remaining_seconds = 0;
static bool counting = false;
static unsigned long last_tick_ms = 0;

/**
 * Режим таймера обратного отсчёта.
 *
 * Фаза установки (timer_setup_phase == true):
 *   KEY1 — больше минут.
 *   KEY2 — меньше минут.
 *   KEY3 — стартовать обратный отсчёт.
 *   KEY4 — выйти к часам.
 *
 * Фаза работы:
 *   KEY3 — пауза/продолжить.
 *   KEY4 — выйти к часам.
 *
 * Когда таймер дотикал до нуля, играет короткая трель из бипов и
 * любая кнопка возвращает программу к часам.
 */
AppMode mode_timer() {
    char line[17];

    byte k = pressed_keys;
    pressed_keys = 0;

    if (timer_setup_phase) {
        led_set(LED_RUN, false);
        display_clear();
        sprintf(line, "Timer set %02d min", set_minutes);
        display_text(0, 0, line);
        display_text(1, 0, "K1+ K2- K3go K4X");

        if (k & KEY_1_MASK) {
            set_minutes = (set_minutes >= 99) ? 99 : set_minutes + 1;
        }
        if (k & KEY_2_MASK) {
            set_minutes = (set_minutes <= 1) ? 1 : set_minutes - 1;
        }
        if (k & KEY_3_MASK) {
            remaining_seconds = (long)set_minutes * 60L;
            counting = true;
            timer_setup_phase = false;
            last_tick_ms = millis();
            led_set(LED_RUN, true);
        }
        if (k & KEY_4_MASK) return MODE_CLOCK;
        return MODE_TIMER;
    }

    // Фаза обратного отсчёта.
    if (counting && remaining_seconds > 0) {
        unsigned long ms = millis();
        if (ms - last_tick_ms >= 1000) {
            last_tick_ms = ms;
            remaining_seconds--;
        }
    }

    long mm = remaining_seconds / 60;
    long ss = remaining_seconds % 60;

    display_clear();
    sprintf(line, "Timer %02ld:%02ld %s", mm, ss, counting ? "RUN" : "PAU");
    display_text(0, 0, line);
    display_text(1, 0, counting ? "K3pause   K4exit" : "K3resume  K4exit");

    if (k & KEY_3_MASK) {
        counting = !counting;
        led_set(LED_RUN, counting);
        if (counting) last_tick_ms = millis();
    }
    if (k & KEY_4_MASK) {
        counting = false;
        timer_setup_phase = true;
        set_minutes = 1;
        remaining_seconds = 0;
        led_set(LED_RUN, false);
        return MODE_CLOCK;
    }

    if (remaining_seconds <= 0 && !counting) {
        // Счёт окончен — небольшая трель из бипов.
        display_clear();
        display_text(0, 0, "  TIME IS UP!");
        for (byte i = 0; i < 3; i++) {
            beep(120);
            delay(100);
        }
        // Ждём любую кнопку.
        while (true) {
            scan_keys();
            byte kk = pressed_keys;
            pressed_keys = 0;
            if (kk) break;
            delay(20);
        }
        timer_setup_phase = true;
        set_minutes = 1;
        remaining_seconds = 0;
        led_set(LED_RUN, false);
        return MODE_CLOCK;
    }

    if (remaining_seconds <= 0 && counting) {
        // Только что досчитали до нуля — следующий проход loop() сработает
        // в ветке выше после установки counting=false.
        counting = false;
        led_set(LED_RUN, false);
    }

    return MODE_TIMER;
}
