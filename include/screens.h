#ifndef SCREENS_H
#define SCREENS_H

#include "context.h"

// Перечисление всех режимов программы.
// Стейт-машина в main.cpp переключается между ними.
enum AppMode {
    MODE_CLOCK,
    MODE_RING,
    MODE_TIMER,
    MODE_STOPWATCH,
    MODE_RESET,
    MODE_SET_TIME,
    MODE_ALARM_SETUP
};

// Каждый режим принимает указатель на общий контекст (всё состояние) и
// возвращает следующий режим. Никаких глобальных переменных, никаких
// static-локалей внутри функций — состояние хранится в ctx->*_s.
AppMode mode_clock(AppContext* ctx);
AppMode mode_ring(AppContext* ctx);
AppMode mode_timer(AppContext* ctx);
AppMode mode_stopwatch(AppContext* ctx);
AppMode mode_reset(AppContext* ctx);
AppMode mode_set_time(AppContext* ctx);
AppMode mode_alarm_setup(AppContext* ctx);

#endif
