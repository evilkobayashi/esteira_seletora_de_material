#include "sensor.h"
#include "config.h"

void sensorInit() {
    pinMode(PIN_TRIG1, OUTPUT);
    pinMode(PIN_ECHO1, INPUT);
    pinMode(PIN_TRIG2, OUTPUT);
    pinMode(PIN_ECHO2, INPUT);
    digitalWrite(PIN_TRIG1, LOW);
    digitalWrite(PIN_TRIG2, LOW);
}

float sensorDistancia(uint8_t pinTrig, uint8_t pinEcho) {
    digitalWrite(pinTrig, LOW);
    delayMicroseconds(2);
    digitalWrite(pinTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(pinTrig, LOW);

    // timeout de 30ms (~5m maxima) para nao travar o loop
    long duracao = pulseIn(pinEcho, HIGH, 30000UL);
    if (duracao == 0) return 999.0f;   // sem eco = fora de alcance
    return duracao * 0.0343f / 2.0f;
}

bool sensorPresenca() {
    return sensorDistancia(PIN_TRIG1, PIN_ECHO1) < DIST_PRESENCA_CM;
}

bool sensorGrande() {
    return sensorDistancia(PIN_TRIG2, PIN_ECHO2) < DIST_GRANDE_CM;
}
