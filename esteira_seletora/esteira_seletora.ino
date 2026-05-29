#include "config.h"
#include "sensor.h"
#include "motor.h"
#include "display.h"

// ── Maquina de estados ───────────────────────────────────────────────────────
//
//  AGUARDANDO ──[objeto detectado]──► CLASSIFICANDO ──[300ms]──► GRANDE ──[150ms]──► REVERTENDO ──[1800ms]──► AGUARDANDO
//                                                            └──► PEQUENO ──[objeto sai]──► AGUARDANDO
//
// Sensor 1 (presença): detecta qualquer objeto na entrada da esteira.
// Sensor 2 (tamanho) : montado a altura intermediaria — leitura abaixo do limiar
//                      indica objeto alto/grande que deve ser desviado.

enum Estado {
    AGUARDANDO,     // esteira em movimento, aguardando objeto
    CLASSIFICANDO,  // objeto detectado, aguardando estabilizacao para medir
    GRANDE,         // objeto grande confirmado, parando antes de reverter
    PEQUENO,        // objeto pequeno, segue pelo trajeto normal
    REVERTENDO      // motor revertendo para desviar objeto grande
};

static Estado   estadoAtual  = AGUARDANDO;
static uint32_t tempoEstado  = 0;
static uint32_t ultimoPoll   = 0;
static uint16_t contGrande   = 0;
static uint16_t contPequeno  = 0;
static uint8_t  debounce     = 0;

void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    sensorInit();
    motorInit();
    displayInit();

    motorForward(VELOCIDADE_MOTOR);
    displayAguardando();
    Serial.println("Sistema iniciado.");
}

void loop() {
    uint32_t agora = millis();

    switch (estadoAtual) {

        // ── Aguardando objeto ────────────────────────────────────────────────
        case AGUARDANDO:
            if (agora - ultimoPoll >= INTERVALO_POLL_MS) {
                ultimoPoll = agora;
                if (sensorPresenca()) {
                    debounce++;
                    if (debounce >= DEBOUNCE_LEITURAS) {
                        debounce    = 0;
                        estadoAtual = CLASSIFICANDO;
                        tempoEstado = agora;
                        displayClassificando();
                        Serial.println("Objeto detectado, classificando...");
                    }
                } else {
                    debounce = 0;
                }
            }
            break;

        // ── Aguarda estabilizacao e mede tamanho ─────────────────────────────
        case CLASSIFICANDO:
            if (agora - tempoEstado >= TEMPO_CLASSIF_MS) {
                if (sensorGrande()) {
                    contGrande++;
                    estadoAtual = GRANDE;
                    tempoEstado = agora;
                    motorStop();
                    displayGrande(contGrande, contPequeno);
                    Serial.print("GRANDE! G="); Serial.print(contGrande);
                    Serial.print(" P="); Serial.println(contPequeno);
                } else {
                    contPequeno++;
                    estadoAtual = PEQUENO;
                    tempoEstado = agora;
                    displayPequeno(contGrande, contPequeno);
                    Serial.print("Pequeno. G="); Serial.print(contGrande);
                    Serial.print(" P="); Serial.println(contPequeno);
                }
            }
            break;

        // ── Objeto grande: pausa breve antes de reverter ──────────────────────
        case GRANDE:
            if (agora - tempoEstado >= TEMPO_PARADA_MS) {
                estadoAtual = REVERTENDO;
                tempoEstado = agora;
                motorReverse(VELOCIDADE_MOTOR);
                displayRevertendo();
                Serial.println("Revertendo para bin grande...");
            }
            break;

        // ── Objeto pequeno: aguarda sair do sensor antes de voltar ao inicio ──
        case PEQUENO:
            if (agora - ultimoPoll >= INTERVALO_POLL_MS) {
                ultimoPoll = agora;
                if (!sensorPresenca()) {
                    estadoAtual = AGUARDANDO;
                    motorForward(VELOCIDADE_MOTOR);
                    displayAguardando();
                    Serial.println("Objeto pequeno saiu. Aguardando...");
                }
            }
            break;

        // ── Reverso concluido: retoma sentido normal ──────────────────────────
        case REVERTENDO:
            if (agora - tempoEstado >= TEMPO_REVERSO_MS) {
                estadoAtual = AGUARDANDO;
                motorForward(VELOCIDADE_MOTOR);
                displayAguardando();
                Serial.println("Reverso concluido. Aguardando...");
            }
            break;
    }
}
