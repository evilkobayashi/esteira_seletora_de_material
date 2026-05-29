#include "display.h"
#include "config.h"
#include <LiquidCrystal.h>

static LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void displayInit() {
    lcd.begin(16, 2);
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
