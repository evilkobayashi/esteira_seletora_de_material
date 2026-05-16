#pragma once

// Pinos motor (L298N)
#define PIN_MOTOR_IN1        4
#define PIN_MOTOR_IN2        5
#define PIN_MOTOR_ENA        9   // PWM — remover jumper ENA do L298N

// Pinos sensor (KY-036)
#define PIN_SENSOR_DIGITAL   2
#define PIN_SENSOR_ANALOG    A0

// OLED (SSD1306 I2C)
#define OLED_ADDRESS         0x3C  // se nao funcionar, tentar 0x3D
#define OLED_WIDTH           128
#define OLED_HEIGHT          64
#define OLED_RESET           -1    // sem pino reset dedicado

// Timing (ms)
#define TEMPO_PARADA_MS       200   // parado antes de reverter
#define TEMPO_REVERSO_MS     1500   // duracao do reverso
#define INTERVALO_POLLING_MS   50   // frequencia de leitura do sensor

// Motor
#define VELOCIDADE_MOTOR      180   // 0-255 PWM

// Debounce
#define DEBOUNCE_LEITURAS       3   // leituras consecutivas HIGH para confirmar metal
