#include "wiring.h"

// 16 ms accuracy
uint32_t millis()
{
  uint32_t m;
  asm volatile ("rcall _millis" : "=w" (m) :: "r30");
  return m;
}

volatile uint32_t timer0_overflow = 0;

ISR(TIM0_OVF_vect)
{
  timer0_overflow++;
}

uint32_t micros()
{
  uint8_t old_sreg = SREG;
  uint8_t timer_counter = TCNT0;
  cli();
  uint32_t overflow_counter = timer0_overflow;
  SREG = old_sreg;

  #if F_CPU == 20000000L
    return ((overflow_counter << 8) + timer_counter) * 3;
  #elif F_CPU == 16000000L
    return ((overflow_counter << 8) + timer_counter) * 4;
  #elif F_CPU == 12000000L
    return ((overflow_counter << 8) + timer_counter) * 5;
  #elif F_CPU == 9600000L
    return ((overflow_counter << 8) + timer_counter) * 7;
  #elif F_CPU == 8000000L
    return ((overflow_counter << 8) + timer_counter) * 8;
  #elif F_CPU == 4800000L
    return ((overflow_counter << 8) + timer_counter) * 13;
  #elif F_CPU == 1200000L
    return ((overflow_counter << 8) + timer_counter) * 7;
  #elif F_CPU == 1000000L
    return ((overflow_counter << 8) + timer_counter) * 8;
  #elif F_CPU == 600000L
    return ((overflow_counter << 8) + timer_counter) * 13;
  #elif F_CPU == 128000L
    return ((overflow_counter << 8) + timer_counter) * 62;
 #endif
}

int16_t analog_read(uint8_t pin)
{
    ADMUX = (ADMUX & 0xFC) | pin;

    #if F_CPU <= 200000L
      ADCSRA = _BV(ADEN) | _BV(ADSC);  // Prescaler 2 -> F_CPU / 2
    #elif F_CPU <= 1200000L
      ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADPS1); // Prescaler 4 -> F_CPU / 4
    #elif F_CPU <= 6400000L
      ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADPS2); // Prescaler 16 -> F_CPU / 16
    #else
      ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // Prescaler 128 -> F_CPU / 128
    #endif

    while (ADCSRA & _BV(ADSC));
      int16_t result = ADCW;
    ADCSRA = 0;
    return result;
}

void init()
{
  #if F_CPU >= 4800000L
    TCCR0B = _BV(CS00) | _BV(CS01); // F_CPU / 64
  #else
    TCCR0B = _BV(CS01);             // F_CPU / 8
  #endif
  TIMSK0 = _BV(TOIE0);
  TCNT0 = 0;
  sei();
}