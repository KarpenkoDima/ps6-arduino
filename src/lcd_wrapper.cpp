#include "lcd_wrapper.h"
#include "config.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

static LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, LCD_COLS, LCD_ROWS);

void display_init() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void display_clear() {
    lcd.clear();
}

void display_text(byte row, byte col, const char* text) {
    lcd.setCursor(col, row);
    lcd.print(text);
}

void display_backlight(bool enabled) {
    if (enabled) {
        lcd.backlight();
    } else {
        lcd.noBacklight();
    }
}
