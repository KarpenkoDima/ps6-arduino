#ifndef LCD_WRAPPER_H
#define LCD_WRAPPER_H

#include <Arduino.h>

// Подготовить LCD к работе и включить подсветку.
void display_init();

// Очистить экран и поставить курсор в начало.
void display_clear();

// Вывести строку в указанной позиции (row 0..1, col 0..15).
void display_text(byte row, byte col, const char* text);

// Включить или выключить подсветку дисплея.
void display_backlight(bool enabled);

#endif
