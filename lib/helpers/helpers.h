#ifndef HELPERS_H
#define HELPERS_H

#include <Arduino.h>

// Безопасно увеличивает значение с переходом через 0 при достижении max.
byte step_up(byte value, byte max_exclusive);

// Безопасно уменьшает значение с переходом на (max-1) при value==0.
byte step_down(byte value, byte max_exclusive);

#endif
