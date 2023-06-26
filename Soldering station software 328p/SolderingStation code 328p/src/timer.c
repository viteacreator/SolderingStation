/*
 * timer.c
 *
 * Created: 3/4/2023 10:00:19 PM
 *  Author: Victor
 */

void timerinit(struct timer* timer) {
  se
  TCC
}

uint8_t checktimer(struct timer* timer,
                   uint32_t intervtime) {  // if time left with set interval,
                                           // return true and reset prevtime
  if (actualtime - timer->prevtime >= intervaltime) {
    timer->prevtime = actualtime;
    return true;
  }
  return false;
}

void resettimer(struct timer* timer) {
  timer->prevtime = actualtime;
}

ISR(TIMER1_OVF_vect){

}