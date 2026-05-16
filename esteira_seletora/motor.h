#pragma once
#include <Arduino.h>

void motorInit();
void motorForward(uint8_t speed);
void motorReverse(uint8_t speed);
void motorStop();
