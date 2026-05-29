#include "display.h"
#include "config.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

static LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

void displayInit() {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ESTEIRA SELETORA");
    lcd.setCursor(0, 1);
    lcd.print("  Iniciando...  ");
    delay(1200);
    lcd.clear();
}

void displayStatus(const char* linha1, const char* linha2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(linha1);
    lcd.setCursor(0, 1);
    lcd.print(linha2);
}

void displayContagens(uint16_t grande, uint16_t pequeno) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Grd:");
    lcd.print(grande);
    lcd.setCursor(8, 0);
    lcd.print("Peq:");
    lcd.print(pequeno);
    lcd.setCursor(0, 1);
    lcd.print("Total:");
    lcd.print(grande + pequeno);
}
