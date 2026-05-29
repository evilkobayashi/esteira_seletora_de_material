#include "display.h"
#include "config.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

static LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

// ── Caracteres customizados (slots CGRAM 0-5) ────────────────────────────────

// Slot 0 — seta direita dupla ►► (esteira avancando)
static const uint8_t ICONE_AVANCAR[8] = {
    0b00000,
    0b01001,
    0b11011,
    0b01001,
    0b00000,
    0b01001,
    0b11011,
    0b01001,
};

// Slot 1 — seta esquerda dupla ◄◄ (esteira revertendo)
static const uint8_t ICONE_REVERTER[8] = {
    0b00000,
    0b10010,
    0b11011,
    0b10010,
    0b00000,
    0b10010,
    0b11011,
    0b10010,
};

// Slot 2 — objeto grande (bloco alto)
static const uint8_t ICONE_GRANDE[8] = {
    0b01110,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
    0b11111,
};

// Slot 3 — objeto pequeno (bloco baixo)
static const uint8_t ICONE_PEQUENO[8] = {
    0b00000,
    0b00000,
    0b00000,
    0b00000,
    0b01110,
    0b11111,
    0b11111,
    0b11111,
};

// Slot 4 — checkmark ✓
static const uint8_t ICONE_OK[8] = {
    0b00000,
    0b00001,
    0b00011,
    0b10110,
    0b11100,
    0b01000,
    0b00000,
    0b00000,
};

// Slot 5 — interrogacao / medindo ?
static const uint8_t ICONE_MEDIR[8] = {
    0b01110,
    0b10001,
    0b00001,
    0b00110,
    0b00100,
    0b00000,
    0b00100,
    0b00000,
};

#define CH_AVANCAR  (char)0
#define CH_REVERTER (char)1
#define CH_GRANDE   (char)2
#define CH_PEQUENO  (char)3
#define CH_OK       (char)4
#define CH_MEDIR    (char)5

// ── Helper: imprime numero com zeros a esquerda (3 digitos) ──────────────────
static void printPadded(uint16_t val) {
    if (val < 100) lcd.print('0');
    if (val <  10) lcd.print('0');
    lcd.print(val);
}

// ── Inicializacao ────────────────────────────────────────────────────────────
void displayInit() {
    lcd.init();
    lcd.backlight();
    lcd.createChar(0, (uint8_t*)ICONE_AVANCAR);
    lcd.createChar(1, (uint8_t*)ICONE_REVERTER);
    lcd.createChar(2, (uint8_t*)ICONE_GRANDE);
    lcd.createChar(3, (uint8_t*)ICONE_PEQUENO);
    lcd.createChar(4, (uint8_t*)ICONE_OK);
    lcd.createChar(5, (uint8_t*)ICONE_MEDIR);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ESTEIRA SELETORA");
    lcd.setCursor(0, 1);
    lcd.print(" Inicializando. ");
    delay(1200);
    lcd.clear();
}

// ── Telas de estado ──────────────────────────────────────────────────────────

void displayAguardando() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(CH_AVANCAR);
    lcd.write(CH_AVANCAR);
    lcd.print(" AGUARDANDO ");
    lcd.write(CH_AVANCAR);
    lcd.write(CH_AVANCAR);
    lcd.setCursor(0, 1);
    lcd.print("  esteira livre ");
}

void displayClassificando() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(CH_MEDIR);
    lcd.print(" CLASSIFICANDO  ");
    lcd.setCursor(0, 1);
    lcd.print(" medindo tamanho");
}

void displayGrande(uint16_t contGrande, uint16_t contPequeno) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(CH_GRANDE);
    lcd.print(" GRANDE  G:");
    printPadded(contGrande);
    lcd.setCursor(0, 1);
    lcd.write(CH_REVERTER);
    lcd.write(CH_REVERTER);
    lcd.print(" desviando  P:");
    printPadded(contPequeno);
}

void displayPequeno(uint16_t contGrande, uint16_t contPequeno) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(CH_PEQUENO);
    lcd.write(CH_OK);
    lcd.print(" PEQUENO P:");
    printPadded(contPequeno);
    lcd.setCursor(0, 1);
    lcd.write(CH_AVANCAR);
    lcd.write(CH_AVANCAR);
    lcd.print(" passando   G:");
    printPadded(contGrande);
}

void displayRevertendo() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(CH_REVERTER);
    lcd.write(CH_REVERTER);
    lcd.print(" REVERTENDO     ");
    lcd.setCursor(0, 1);
    lcd.print("  >> bin GRANDE ");
}

void displayContagens(uint16_t grande, uint16_t pequeno) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(CH_GRANDE);
    lcd.print(" Grd:");
    printPadded(grande);
    lcd.setCursor(9, 0);
    lcd.write(CH_PEQUENO);
    lcd.print("Peq:");
    printPadded(pequeno);
    lcd.setCursor(0, 1);
    lcd.print("   Total: ");
    printPadded(grande + pequeno);
    lcd.print("   ");
}
