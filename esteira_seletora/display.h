#pragma once
#include <Arduino.h>

void displayInit();
void displayStatus(const char* linha1, const char* linha2);
void displayContagens(uint16_t grande, uint16_t pequeno);
