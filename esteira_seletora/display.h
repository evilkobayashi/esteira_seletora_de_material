#pragma once
#include <Arduino.h>

bool displayInit();
void displayStatus(const char* status);
void displayCounts(uint16_t metalCount);
