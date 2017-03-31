/*
 * ctm1284p.c
 * 
 * hardware specific clocktimer-code for atmega1284p
 * (an avr51 family member)
 * 
 * !!    not to be compiled directly    !!
 * !! see clocktimer.h and clocktimer.c !!
 * 
 * For timekeeping initialize the 16bit timer3
 * on atmega1284p.
 * This driver occupies timer1 fully exclusivly!
 * 
 */
#define __CTM1284p_C_08bc8b223b334fccb47fc2544daa8f80	1

#include <avr/io.h>


/* 
 * symbols to satisfy hwclock: 
 * 	__hwclock_timer_init
 * 	__hwclock_timer_start
 * 	__hwclock_timer_stop
 * 	__hwclock_timer_final
 */

void __hwclock_timer_init(void) {
  TCCR3A	=  0;
  TCCR3B	=  0;
  TCCR3C	=  0;
  TCNT3H	=  0;
  TCNT3L	=  0;
}

void __hwclock_timer_start(void) {
  TCCR3B	|= CLOCKTIMER_PRESCALER_CODE;
}

void __hwclock_timer_stop(void) {
  TCCR3B	=  0;  
}

void __hwclock_timer_final(void) {
}
