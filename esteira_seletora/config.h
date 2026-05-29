#pragma once

// ── Motor (L298N) ────────────────────────────────────────────────────────────
#define PIN_MOTOR_IN1        4
#define PIN_MOTOR_IN2        5
#define PIN_MOTOR_ENA        3   // PWM — remover jumper ENA do L298N

// ── Sensor ultrassonico 1 — presença (HC-SR04) ───────────────────────────────
#define PIN_TRIG1            6
#define PIN_ECHO1            7

// ── Sensor ultrassonico 2 — tamanho (HC-SR04) ────────────────────────────────
#define PIN_TRIG2           10
#define PIN_ECHO2           11

// ── LCD 16x2 com modulo I2C ──────────────────────────────────────────────────
// SDA → A4, SCL → A5 (fixos no Arduino Uno, nao alterar)
#define LCD_I2C_ADDR        0x27  // endereco I2C do modulo (tentar 0x3F se nao funcionar)
#define LCD_COLS            16
#define LCD_ROWS             2

// ── Thresholds ultrassonicos (cm) ────────────────────────────────────────────
#define DIST_PRESENCA_CM    15   // sensor 1: objeto detectado se dist < este valor
#define DIST_GRANDE_CM       8   // sensor 2: objeto "grande" se dist < este valor

// ── Timing (ms) ──────────────────────────────────────────────────────────────
#define TEMPO_CLASSIF_MS    300  // aguarda objeto estabilizar antes de medir tamanho
#define TEMPO_PARADA_MS     150  // pausa antes de reverter
#define TEMPO_REVERSO_MS   1800  // duracao do reverso para desviar objeto grande
#define INTERVALO_POLL_MS    60  // frequencia de leitura dos sensores

// ── Motor ────────────────────────────────────────────────────────────────────
#define VELOCIDADE_MOTOR    180  // 0-255 PWM

// ── Debounce ─────────────────────────────────────────────────────────────────
#define DEBOUNCE_LEITURAS     3  // leituras consecutivas para confirmar presenca
