/*
 * ctm8.c
 * 
 * controller specific clocktimer-code for atmega8
 * 
 * !!    not to be compiled directly    !!
 * !! see clocktimer.h and clocktimer.c !!
 * 
 * For timekeeping initialize the 16bit timer1
 * on atmega8.
 * This driver occupies timer1 fully exclusivly!
 * 
 */
#define __CTM8_C_08bc8b223b334fccb47fc2544daa8f80	1

#include <avr/io.h>


/* 
 * symbols to satisfy hwclock: 
 * 	__hwclock_timer_init
 * 	__hwclock_timer_start
 * 	__hwclock_timer_stop
 * 	__hwclock_timer_final
 */

void __hwclock_timer_init(void) {
  TCCR1A	=  0;
  TCCR1B	=  0;
  TCNT1H	=  0;
  TCNT1L	=  0;
}

void __hwclock_timer_start(void) {
  TCCR1B	|= CLOCKTIMER_PRESCALER_CODE;
}

void __hwclock_timer_stop(void) {
  TCCR1B	=  0;  
}

void __hwclock_timer_final(void) {
}
