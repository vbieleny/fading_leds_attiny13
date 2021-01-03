#include "fast_random.h"

uint8_t rand_value;

void fast_random_seed(uint8_t seed)
{
  rand_value = seed;
}

uint8_t fast_random()
{
  rand_value = (rand_value * 109 + 89) % 251;
  return rand_value;
}

uint8_t fast_random_max(uint8_t max)
{
  return fast_random(0, max);
}

uint8_t fast_random_min_max(uint8_t min, uint8_t max)
{
  return min + fast_random() % (max - min);
}