/*
 * adc.c
 *
 * Created: 3/5/2023 3:06:27 PM
 *  Author: Victor
 */
#include "adc.h"

void adcInit(uint8_t ref) {
  if (INTERNAL) {
    ADMUX |= (1 << REFS0);   // Set REFS0 to 1
    ADMUX &= ~(1 << REFS1);  // Set REFS1 to 0
  } else if (VCC || VDD) {
    ADMUX &= ~(1 << REFS0);  // Set REFS0 to 0
    ADMUX &= ~(1 << REFS1);  // Set REFS1 to 0
  } else if (AREF) {
    ADMUX |= (1 << REFS0);  // Set REFS0 to 1
    ADMUX |= (1 << REFS1);  // Set REFS1 to 1
  }

  ADCSRA |= (1 << ADEN);  // ADCSRA = 0x80 (enable ADC interupt) and start
                          // conversion (01000000)
  ADCSRB = 0x00;          // control start register
}

uint16_t adcRead(uint8_t channel) {
  ADMUX = channel;        // set chanel
  ADCSRA |= (1 << ADSC);  // ADCSRA |= 0x40;
  while (!(ADCSRA & (1 << ADIF)));
  ADCSRA |= 0x10;
  return ADCW;
}