#ifndef CONTEXT_H
#define CONTEXT_H

#include <Arduino.h>

// Битовая маска нажатых кнопок: каждый бит — отдельная кнопка
// (KEY_1_MASK, KEY_2_MASK, KEY_3_MASK, KEY_4_MASK из config.h).
extern volatile byte pressed_keys;

// Опрашивает все кнопки, применяет программный антидребезг и сохраняет
// результат в pressed_keys. Учитываются только новые нажатия —
// удержание кнопки повторно не срабатывает.
void scan_keys();

#endif
