#include <Arduino.h>

#include "config.h"
#include "context.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "sensors.h"
#include "screens.h"
#include "screen_common.h"

static const byte key_pins[4]  = { KEY_PIN_1, KEY_PIN_2, KEY_PIN_3, KEY_PIN_4 };
static const byte key_masks[4] = { KEY_1_MASK, KEY_2_MASK, KEY_3_MASK, KEY_4_MASK };

void scan_keys(AppContext* ctx) {
    byte fresh = 0;
    unsigned long ms = millis();

    for (byte i = 0; i < 4; i++) {
        bool down_now = digitalRead(key_pins[i]) == LOW;

        if (down_now && !ctx->keyboard.was_down[i]) {
            if (ms - ctx->keyboard.last_press_ms[i] >= KEY_DEBOUNCE_MS) {
                fresh |= key_masks[i];
                ctx->keyboard.last_press_ms[i] = ms;
            }
        }

        ctx->keyboard.was_down[i] = down_now;
    }

    if (fresh) {
        ctx->pressed_keys |= fresh;
    }
}

// Единственная точка, где хранится всё состояние приложения.
// static ограничивает видимость этим файлом — снаружи недоступно.
static AppContext app_ctx;
static AppMode    current_mode = MODE_CLOCK;

void setup() {
    Serial.begin(9600);

    pinMode(KEY_PIN_1, INPUT_PULLUP);
    pinMode(KEY_PIN_2, INPUT_PULLUP);
    pinMode(KEY_PIN_3, INPUT_PULLUP);
    pinMode(KEY_PIN_4, INPUT_PULLUP);

    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    leds_init();
    sensors_env_init();
    display_init();
    chrono_init();
    alarm_load(&app_ctx);  // заполняет app_ctx.wake_alarm из EEPROM
}

void loop() {
    scan_keys(&app_ctx);
    leds_tick();

    switch (current_mode) {
        case MODE_CLOCK:       current_mode = mode_clock(&app_ctx);       break;
        case MODE_RING:        current_mode = mode_ring(&app_ctx);        break;
        case MODE_TIMER:       current_mode = mode_timer(&app_ctx);       break;
        case MODE_STOPWATCH:   current_mode = mode_stopwatch(&app_ctx);   break;
        case MODE_RESET:       current_mode = mode_reset(&app_ctx);       break;
        case MODE_SET_TIME:    current_mode = mode_set_time(&app_ctx);    break;
        case MODE_ALARM_SETUP: current_mode = mode_alarm_setup(&app_ctx); break;
    }
}
