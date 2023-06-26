/*
 * button.h
 *
 * Created: 3/4/2023 10:02:49 PM
 *  Author: Victor
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "button.c"

struct button {
    uint8_t _pin;
    uint32_t _tmr;
    uint32_t _tmr_hold;
    uint8_t _flag;
};

void buttinit(byte pin);
// bool click();
uint8_t check(struct button* butt, uint8_t hold_delay);
uint8_t check(struct button* butt);

#endif /* BUTTON_H_ */