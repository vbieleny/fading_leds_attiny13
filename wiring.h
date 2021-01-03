#pragma once

#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/common.h>

uint32_t millis();
uint32_t micros();
int16_t analog_read(uint8_t pin);
void init();