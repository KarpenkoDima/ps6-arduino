#ifndef CONFIG_H
#define CONFIG_H

// ===== Подключение часов реального времени DS1302 =====
#define RTC_RST_PIN 9
#define RTC_DAT_PIN 4
#define RTC_CLK_PIN 5

// ===== Звуковой сигнал =====
#define BUZZER_PIN 6
#define BUZZER_FREQ 1500

// ===== Световая индикация (две лампочки) =====
// Первая лампочка горит, когда включен будильник.
// Вторая лампочка горит, когда активен таймер или секундомер.
// Во время звонка будильника обе лампочки мигают по очереди.
#define LED_ALARM_PIN 2
#define LED_RUN_PIN   3

// ===== Дисплей LCD 16x2 через I2C =====
// Линии I2C на Arduino Uno фиксированы: SDA=A4, SCL=A5.
#define LCD_I2C_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// ===== Кнопки =====
#define KEY_PIN_1  7
#define KEY_PIN_2 10
#define KEY_PIN_3 11
#define KEY_PIN_4 12

#define KEY_1_MASK 0x01
#define KEY_2_MASK 0x02
#define KEY_3_MASK 0x04
#define KEY_4_MASK 0x08

// Минимальный интервал между двумя засчитанными нажатиями одной кнопки.
#define KEY_DEBOUNCE_MS 80

// ===== Стартовое время будильника по умолчанию =====
#define DEFAULT_WAKE_HOUR    7
#define DEFAULT_WAKE_MINUTE  0

// ===== Стартовое время RTC (используется только при первой прошивке
// или если на модуле села батарейка и время потерялось) =====
#define RTC_BOOT_DAY    23
#define RTC_BOOT_MONTH  5
#define RTC_BOOT_YEAR   2026
#define RTC_BOOT_HOUR   12
#define RTC_BOOT_MINUTE 0
#define RTC_BOOT_SECOND 0

// Удержание кнопки сброса для срабатывания factory reset (мс).
#define RESET_HOLD_MS 3000

#endif
