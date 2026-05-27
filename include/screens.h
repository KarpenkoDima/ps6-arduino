#ifndef SCREENS_H
#define SCREENS_H

// Перечисление всех режимов программы.
// Стейт-машина в main.cpp переключается между ними.
enum AppMode {
    MODE_CLOCK,
    MODE_RING,
    MODE_TIMER,
    MODE_STOPWATCH,
    MODE_RESET,
    MODE_SET_TIME,     // установка текущего времени и даты
    MODE_ALARM_SETUP   // настройка будильника (без while-true)
};

// Каждый режим живёт в собственном файле в src/screens.
// Функция возвращает следующий режим, в который надо перейти.
AppMode mode_clock();
AppMode mode_ring();
AppMode mode_timer();
AppMode mode_stopwatch();
AppMode mode_reset();
AppMode mode_set_time();
AppMode mode_alarm_setup();

#endif
