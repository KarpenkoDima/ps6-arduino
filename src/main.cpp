#include <Arduino.h>

#include "config.h"
#include "context.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "sensors.h"
#include "screens.h"
#include "screen_common.h"

volatile byte pressed_keys = 0;

// Для каждой кнопки храним момент последнего засчитанного нажатия,
// чтобы убрать «дребезг» физического контакта.
static unsigned long last_press_ms[4] = { 0, 0, 0, 0 };
static bool was_down[4] = { false, false, false, false };

static const byte key_pins[4]  = { KEY_PIN_1, KEY_PIN_2, KEY_PIN_3, KEY_PIN_4 };
static const byte key_masks[4] = { KEY_1_MASK, KEY_2_MASK, KEY_3_MASK, KEY_4_MASK };

/**
 * Опрос кнопок с программным антидребезгом по времени.
 * Засчитываем нажатие только если кнопка была отпущена и с момента
 * последнего нажатия прошло не меньше KEY_DEBOUNCE_MS миллисекунд.
 */
void scan_keys() {
    byte fresh = 0;
    unsigned long ms = millis();

    for (byte i = 0; i < 4; i++) {
        bool down_now = digitalRead(key_pins[i]) == LOW;

        if (down_now && !was_down[i]) {
            if (ms - last_press_ms[i] >= KEY_DEBOUNCE_MS) {
                fresh |= key_masks[i];
                last_press_ms[i] = ms;
            }
        }

        was_down[i] = down_now;
    }

    if (fresh) {
        pressed_keys |= fresh;
    }
}

/**
 * Точка входа Arduino.
 * Настраивает все периферийные модули будильника:
 * кнопки, LCD, RTC, светодиоды, буззер.
 */
void setup() {
    Serial.begin(9600);

    pinMode(KEY_PIN_1, INPUT_PULLUP);
    pinMode(KEY_PIN_2, INPUT_PULLUP);
    pinMode(KEY_PIN_3, INPUT_PULLUP);
    pinMode(KEY_PIN_4, INPUT_PULLUP);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    leds_init();
    display_init();
    chrono_init();
    alarm_load();
}

// Текущий режим стейт-машины. Каждый проход loop() вызывает
// функцию текущего режима и переключается на тот, который вернула функция.
static AppMode current_mode = MODE_CLOCK;

/**
 * Главный цикл. Опрашивает кнопки, обновляет «звон» светодиодов
 * и вызывает текущий режим.
 */
void loop() {
    scan_keys();
    leds_tick();

    switch (current_mode) {
        case MODE_CLOCK:     current_mode = mode_clock();     break;
        case MODE_RING:      current_mode = mode_ring();      break;
        case MODE_TIMER:     current_mode = mode_timer();     break;
        case MODE_STOPWATCH: current_mode = mode_stopwatch(); break;
        case MODE_RESET:     current_mode = mode_reset();     break;
    }
}
