#include "config.h"
#include "sensor.h"
#include "motor.h"
#include "display.h"

enum Estado { DETECTANDO, METAL_DETECTADO, REVERTENDO };

static Estado   estadoAtual = DETECTANDO;
static uint32_t tempoEstado = 0;
static uint32_t ultimoPoll  = 0;
static uint16_t contMetal   = 0;
static uint8_t  debounce    = 0;

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    sensorInit();
    motorInit();

    if (!displayInit()) {
        Serial.println("ERRO: OLED falhou. Verificar fiacao I2C.");
        while (true) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            delay(300);
        }
    }

    motorForward(VELOCIDADE_MOTOR);
    displayStatus("DETECTANDO");
    displayCounts(contMetal);
    Serial.println("Sistema iniciado.");
}

void loop() {
    uint32_t agora = millis();

    switch (estadoAtual) {

        case DETECTANDO:
            if (agora - ultimoPoll >= INTERVALO_POLLING_MS) {
                ultimoPoll = agora;
                if (sensorDigital()) {
                    debounce++;
                    Serial.print("Debounce: "); Serial.println(debounce);
                    if (debounce >= DEBOUNCE_LEITURAS) {
                        debounce    = 0;
                        contMetal++;
                        estadoAtual = METAL_DETECTADO;
                        tempoEstado = agora;
                        motorStop();
                        displayStatus("METAL!");
                        displayCounts(contMetal);
                        Serial.print("Metal detectado! Total: "); Serial.println(contMetal);
                    }
                } else {
                    debounce = 0;
                }
            }
            break;

        case METAL_DETECTADO:
            if (agora - tempoEstado >= TEMPO_PARADA_MS) {
                estadoAtual = REVERTENDO;
                tempoEstado = agora;
                motorReverse(VELOCIDADE_MOTOR);
                displayStatus("REVERTENDO");
                Serial.println("Revertendo...");
            }
            break;

        case REVERTENDO:
            if (agora - tempoEstado >= TEMPO_REVERSO_MS) {
                estadoAtual = DETECTANDO;
                motorForward(VELOCIDADE_MOTOR);
                displayStatus("DETECTANDO");
                Serial.println("Detectando...");
            }
            break;
    }
}
