#include "sensors.h"
#include "config.h"
#include <DHT.h>

// ===== DHT11 =====
static DHT dht(DHT_PIN, DHT11);

void sensors_env_init() {
    dht.begin();
}

float read_temperature() {
    return dht.readTemperature();
}

float read_humidity() {
    return dht.readHumidity();
}

// ===== Светодиоды =====
// Внутреннее состояние режима «звона»: когда true, две лампочки
// мигают противофазой каждые 300 мс.
static bool blinking = false;
static unsigned long last_toggle_ms = 0;
static bool blink_phase = false;

static byte pin_for(LedId id) {
    return (id == LED_ALARM) ? LED_ALARM_PIN : LED_RUN_PIN;
}

void leds_init() {
    pinMode(LED_ALARM_PIN, OUTPUT);
    pinMode(LED_RUN_PIN,   OUTPUT);
    digitalWrite(LED_ALARM_PIN, LOW);
    digitalWrite(LED_RUN_PIN,   LOW);
}

void led_set(LedId id, bool on) {
    digitalWrite(pin_for(id), on ? HIGH : LOW);
}

void leds_alarm_blink(bool active) {
    blinking = active;
    if (!active) {
        digitalWrite(LED_ALARM_PIN, LOW);
        digitalWrite(LED_RUN_PIN,   LOW);
        blink_phase = false;
    } else {
        last_toggle_ms = millis();
    }
}

void leds_tick() {
    if (!blinking) return;
    unsigned long ms = millis();
    if (ms - last_toggle_ms >= 300) {
        last_toggle_ms = ms;
        blink_phase = !blink_phase;
        digitalWrite(LED_ALARM_PIN, blink_phase ? HIGH : LOW);
        digitalWrite(LED_RUN_PIN,   blink_phase ? LOW  : HIGH);
    }
}
