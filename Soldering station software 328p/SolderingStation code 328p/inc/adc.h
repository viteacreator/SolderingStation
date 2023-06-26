/*
 * adc.h
 *
 * Created: 3/5/2023 3:06:17 PM
 *  Author: Victor
 */ 


#ifndef ADC_H_
#define ADC_H_
#include <stdint.h>
#include <avr/io.h>

enum {
	INTERNAL,
	VCC,
	VDD,
	AREF
};

void adcInit(uint8_t ref);

uint16_t adcRead(uint8_t channel);

#endif /* ADC_H_ */