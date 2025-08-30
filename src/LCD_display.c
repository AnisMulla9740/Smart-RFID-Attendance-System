#include "lcd_display.h"
#include <LiquidCrystal_I2C.h>
#include "config.h"

static LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

void lcd_init(){
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void lcd_print_line(int row, const char* text){
  lcd.clear();
  lcd.setCursor(0, row);
  lcd.print(text);
}
