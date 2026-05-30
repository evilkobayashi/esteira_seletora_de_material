#include "display.h"
#include "config.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

static LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

// ── Caracteres customizados (slots CGRAM 0-5) ────────────────────────────────

// Slot 0 — seta direita dupla ►► (esteira avancando)
static const uint8_t ICONE_AVANCAR[8] = {
    0b00000, 0b01001, 0b11011, 0b01001,
    0b00000, 0b01001, 0b11011, 0b01001,
};

// Slot 1 — seta esquerda dupla ◄◄ (esteira revertendo)
static const uint8_t ICONE_REVERTER[8] = {
    0b00000, 0b10010, 0b11011, 0b10010,
    0b00000, 0b10010, 0b11011, 0b10010,
};

// Slot 2 — objeto grande (bloco alto)
static const uint8_t ICONE_GRANDE[8] = {
    0b01110, 0b11111, 0b11111, 0b11111,
    0b11111, 0b11111, 0b11111, 0b11111,
};

// Slot 3 — objeto pequeno (bloco baixo)
static const uint8_t ICONE_PEQUENO[8] = {
    0b00000, 0b00000, 0b00000, 0b00000,
    0b01110, 0b11111, 0b11111, 0b11111,
};

// Slot 4 — checkmark
static const uint8_t ICONE_OK[8] = {
    0b00000, 0b00001, 0b00011, 0b10110,
    0b11100, 0b01000, 0b00000, 0b00000,
};

// Slot 5 — interrogacao (medindo)
static const uint8_t ICONE_MEDIR[8] = {
    0b01110, 0b10001, 0b00001, 0b00110,
    0b00100, 0b00000, 0b00100, 0b00000,
};

#define CH_AVANCAR  ((uint8_t)0)
#define CH_REVERTER ((uint8_t)1)
#define CH_GRANDE   ((uint8_t)2)
#define CH_PEQUENO  ((uint8_t)3)
#define CH_OK       ((uint8_t)4)
#define CH_MEDIR    ((uint8_t)5)

// ── Helper: numero com zeros a esquerda, 3 digitos ───────────────────────────
static void p3(uint16_t v) {
    if (v < 100) lcd.print('0');
    if (v <  10) lcd.print('0');
    lcd.print(v);
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

    // createChar() deixa cursor no CGRAM; home() retorna ao DDRAM
    lcd.home();
    lcd.clear();

    lcd.setCursor(0, 0); lcd.print("ESTEIRA SELETORA");
    lcd.setCursor(0, 1); lcd.print(" Inicializando. ");
    delay(1200);
    lcd.clear();
}

// ── Telas — sem lcd.clear() para evitar pisca ────────────────────────────────
// Cada linha ocupa exatamente 16 celulas (contagem nos comentarios).

void displayAguardando() {
    // L0: [I][I] AGUARDANDO [I][I]  = 2+12+2 = 16
    lcd.setCursor(0, 0);
    lcd.write(CH_AVANCAR); lcd.write(CH_AVANCAR);
    lcd.print(" AGUARDANDO ");
    lcd.write(CH_AVANCAR); lcd.write(CH_AVANCAR);
    // L1: "  esteira livre "           = 16
    lcd.setCursor(0, 1);
    lcd.print("  esteira livre ");
}

void displayClassificando() {
    // L0: [I]" CLASSIFICANDO "         = 1+15 = 16
    lcd.setCursor(0, 0);
    lcd.write(CH_MEDIR);
    lcd.print(" CLASSIFICANDO ");
    // L1: " medindo tamanho"           = 16
    lcd.setCursor(0, 1);
    lcd.print(" medindo tamanho");
}

void displayGrande(uint16_t contGrande, uint16_t contPequeno) {
    // L0: [I]" GRANDE  G:"[3]" "       = 1+11+3+1 = 16
    lcd.setCursor(0, 0);
    lcd.write(CH_GRANDE);
    lcd.print(" GRANDE  G:");
    p3(contGrande);
    lcd.print(' ');
    // L1: [I][I]" desviou P:"[3]       = 2+11+3 = 16
    lcd.setCursor(0, 1);
    lcd.write(CH_REVERTER); lcd.write(CH_REVERTER);
    lcd.print(" desviou P:");
    p3(contPequeno);
}

void displayPequeno(uint16_t contGrande, uint16_t contPequeno) {
    // L0: [I]" "[I]" PEQNO P:"[3]" "   = 1+1+1+9+3+1 = 16
    lcd.setCursor(0, 0);
    lcd.write(CH_PEQUENO);
    lcd.print(' ');
    lcd.write(CH_OK);
    lcd.print(" PEQNO P:");
    p3(contPequeno);
    lcd.print(' ');
    // L1: [I][I]" pass.  G:"[3]" "     = 2+10+3+1 = 16
    lcd.setCursor(0, 1);
    lcd.write(CH_AVANCAR); lcd.write(CH_AVANCAR);
    lcd.print(" pass.  G:");
    p3(contGrande);
    lcd.print(' ');
}

void displayRevertendo() {
    // L0: [I][I]" REVERTENDO   "       = 2+14 = 16
    lcd.setCursor(0, 0);
    lcd.write(CH_REVERTER); lcd.write(CH_REVERTER);
    lcd.print(" REVERTENDO   ");
    // L1: "  >> bin GRANDE "           = 16
    lcd.setCursor(0, 1);
    lcd.print("  >> bin GRANDE ");
}

void displayContagens(uint16_t grande, uint16_t pequeno) {
    // L0: [I]"Grd:"[3]" "[I]"P:"[3]" " = 1+4+3+1+1+2+3+1 = 16
    lcd.setCursor(0, 0);
    lcd.write(CH_GRANDE);
    lcd.print("Grd:");
    p3(grande);
    lcd.print(' ');
    lcd.write(CH_PEQUENO);
    lcd.print("P:");
    p3(pequeno);
    lcd.print(' ');
    // L1: "   Total: "[3]"   "         = 10+3+3 = 16
    lcd.setCursor(0, 1);
    lcd.print("   Total: ");
    p3(grande + pequeno);
    lcd.print("   ");
}
