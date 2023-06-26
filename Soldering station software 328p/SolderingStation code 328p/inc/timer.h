/*
 * timer.h
 *
 * Created: 3/4/2023 10:00:19 PM
 *  Author: Victor
 */

#ifndef TIMER_H_
#define TIMER_H_
#include "timer.c"

uin32_t actualtime = 0;  // keeps time in ms

struct timer {
  uint32_t prevtime;
};

// void timerinit(struct timer* timer);

uint8_t checktimer(struct timer* timer,
                   uint32_t intervtime);  // if time left with set interval,
                                          // return true and reset prevtime
void resettimer(struct timer* timer);

#endif /* TIMER_H_ */