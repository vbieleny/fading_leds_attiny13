#include <avr/io.h>
#include "wiring.h"
#include "fast_random.h"

#define HIGH 1
#define LOW 0

// Analog pins
#define A0 0x00     // PB5
#define A1 0x01     // PB2
#define A2 0x02     // PB4
#define A3 0x03     // PB3

#define PIN_COUNT 3
#define INTERVAL 6000UL
#define CHANGE_DELAY 10UL

#define PWM_MAX 255
#define MICRO_INTERVAL 60

uint32_t current_micros = 0;
uint32_t last_micros = 0;

struct pwm_pin
{
  uint8_t pin_number;
  uint8_t pwm_value;
  uint8_t pwm_tick_count;
  uint8_t pwm_pin_state;

  uint8_t pin_state;
};

static void init_pin_default(struct pwm_pin *pin)
{
  pin->pwm_value = 0;
  pin->pwm_tick_count = 0;
  pin->pwm_pin_state = HIGH;

  pin->pin_state = HIGH;
}

static void handle_pwm(struct pwm_pin *pins)
{
  if (current_micros - last_micros >= MICRO_INTERVAL)
  {
    for (uint8_t i = 0; i < PIN_COUNT; ++i)
    {
      if (pins[i].pwm_value == PWM_MAX)
        PORTB |= _BV(pins[i].pin_number);
      else if (pins[i].pwm_value == 0)
        PORTB &= ~_BV(pins[i].pin_number);
      else
      {
        ++(pins[i].pwm_tick_count);
        if (pins[i].pwm_pin_state == HIGH)
        {
          if (pins[i].pwm_tick_count >= pins[i].pwm_value)
            pins[i].pwm_pin_state = LOW;
        }
        else
        {
          if (pins[i].pwm_tick_count >= PWM_MAX)
          {
            pins[i].pwm_pin_state = HIGH;
            pins[i].pwm_tick_count = 0;
          }
        }
        if (pins[i].pwm_pin_state == HIGH)
          PORTB |= _BV(pins[i].pin_number);
        else
          PORTB &= ~_BV(pins[i].pin_number);
      }
    }
    last_micros = current_micros;
  }
}

uint8_t enabled_pin_count(struct pwm_pin *pins)
{
  uint8_t count = 0;
  for (uint8_t i = 0; i < PIN_COUNT; ++i)
  {
    if (pins[i].pin_state == HIGH)
      ++count;
  }
  return count;
}

int main(void)
{
  init();
  fast_random_seed(analog_read(A3) % 255);
  DDRB |= _BV(DDB0) | _BV(DDB1) | _BV(DDB2);
  
  struct pwm_pin pins[PIN_COUNT];
  for (uint8_t i = 0; i < PIN_COUNT; ++i)
    init_pin_default(&pins[i]);

  pins[0].pin_number = PB0;
  pins[1].pin_number = PB1;
  pins[2].pin_number = PB2;

  uint32_t last_step_micros = 0UL;
  uint32_t last_update_time = 0UL;
  while (1)
  {
    current_micros = micros();
    handle_pwm(pins);
    if (current_micros - last_step_micros >= CHANGE_DELAY * 1000UL)
    {
      for (uint8_t i = 0; i < PIN_COUNT; ++i)
      {
        if (current_micros - last_update_time >= INTERVAL * 1000UL)
        {
          pins[i].pin_state = enabled_pin_count(pins) > 1 ? fast_random() < 128 : HIGH;
          if (i == PIN_COUNT - 1)
            last_update_time = current_micros;
        }
        if (pins[i].pin_state == HIGH)
        {
          if (pins[i].pwm_value < 255)
            ++(pins[i].pwm_value);
        }
        else
        {
          if (pins[i].pwm_value > 0)
            --(pins[i].pwm_value);
        }
      }
      last_step_micros = current_micros;
    }
  }
}