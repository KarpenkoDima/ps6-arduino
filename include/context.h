#ifndef CONTEXT_H
#define CONTEXT_H

#include <Arduino.h>
#include <math.h>
#include "config.h"
#include "rtc_wrapper.h"

// ─── Подструктуры состояния ──────────────────────────────────────────────────

struct Alarm {
    byte hour    = DEFAULT_WAKE_HOUR;
    byte minute  = DEFAULT_WAKE_MINUTE;
    bool enabled = false;
};

struct KeyboardState {
    unsigned long last_press_ms[4] = {0, 0, 0, 0};
    bool          was_down[4]      = {false, false, false, false};
};

struct ClockState {
    unsigned long last_display_ms         = 0;
    unsigned long last_sensor_ms          = 0;
    float         last_temp               = NAN;
    float         last_hum                = NAN;
    bool          alarm_fired_this_minute = false;
};

struct AlarmSetupState {
    byte          field       = 0;
    bool          initialized = false;
    unsigned long last_update = 0;
};

struct SetTimeState {
    byte          field       = 0;
    bool          initialized = false;
    DateTime      edit        = {};
    unsigned long last_update = 0;
};

struct TimerState {
    int           set_minutes       = 1;
    long          remaining_seconds = 0;
    bool          counting          = false;
    unsigned long last_tick_ms      = 0;
    bool          setup_phase       = true;
};

struct StopwatchState {
    bool          running        = false;
    unsigned long started_at     = 0;
    unsigned long accumulated_ms = 0;
};

struct ResetState {
    unsigned long hold_start_ms = 0;
};

// ─── Главный контекст приложения ─────────────────────────────────────────────
//
// Единственная структура, которая владеет ВСЕМ состоянием программы.
// Объявляется как static в main.cpp — не видна снаружи файла.
// Любой экран может сбросить своё состояние одной строчкой, например:
//   ctx->clock_s = ClockState{};
//
struct AppContext {
    volatile byte  pressed_keys  = 0;

    Alarm          wake_alarm;      // настройки будильника (час, минута, вкл)
    KeyboardState  keyboard;        // антидребезг кнопок
    ClockState     clock_s;         // главный экран часов
    AlarmSetupState alarm_setup_s;  // экран настройки будильника
    SetTimeState   set_time_s;      // экран установки времени/даты
    TimerState     timer_s;         // таймер обратного отсчёта
    StopwatchState stopwatch_s;     // секундомер
    ResetState     reset_s;         // заводской сброс
};

// Опрос кнопок с программным антидребезгом; читает/пишет через ctx.
void scan_keys(AppContext* ctx);

#endif
