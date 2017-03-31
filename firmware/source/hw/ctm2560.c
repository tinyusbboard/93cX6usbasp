/*
 * ctm2560.c
 * 
 * hardware specific clocktimer-code for atmega2560
 * (and other avr6 family members)
 * 
 * !!    not to be compiled directly    !!
 * !! see clocktimer.h and clocktimer.c !!
 * 
 * For timekeeping initialize the 16bit timer5
 * on atmega2560.
 * This driver occupies timer1 fully exclusivly!
 * 
 */
#define __CTM2560_C_08bc8b223b334fccb47fc2544daa8f80	1

#include <avr/io.h>


/* 
 * symbols to satisfy hwclock: 
 * 	__hwclock_timer_init
 * 	__hwclock_timer_start
 * 	__hwclock_timer_stop
 * 	__hwclock_timer_final
 */

void __hwclock_timer_init(void) {
  TCCR5A	=  0;
  TCCR5B	=  0;
  TCCR5C	=  0;
  TCNT5H	=  0;
  TCNT5L	=  0;
}

void __hwclock_timer_start(void) {
  TCCR5B	|= CLOCKTIMER_PRESCALER_CODE;
}

void __hwclock_timer_stop(void) {
  TCCR5B	=  0;  
}

void __hwclock_timer_final(void) {
}
