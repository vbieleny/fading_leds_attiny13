#pragma once

#include <avr/common.h>

void fast_random_seed(uint8_t seed);
uint8_t fast_random();
uint8_t fast_random_max(uint8_t max);
uint8_t fast_random_min_max(uint8_t min, uint8_t max);