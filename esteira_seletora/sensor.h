#pragma once
#include <Arduino.h>

void   sensorInit();
float  sensorDistancia(uint8_t pinTrig, uint8_t pinEcho);  // retorna cm
bool   sensorPresenca();   // sensor 1: objeto na zona de entrada
bool   sensorGrande();     // sensor 2: objeto acima do limiar de tamanho
