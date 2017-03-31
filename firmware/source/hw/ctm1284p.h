/*
 * ctm1284p.h
 * 
 * hardware specific clocktimer-include for atmega1284p
 * (avr51 family member)
 * 
 */
#ifndef __CTM1284p_H_08bc8b223b334fccb47fc2544daa8f80
#define __CTM1284p_H_08bc8b223b334fccb47fc2544daa8f80	1


#include <avr/io.h>

#	define HWCLOCK_LSBTIMER_USEIOACCESS	0
#	define HWCLOCK_MSBTIMER_USEIOACCESS	0

#	define HWCLOCK_LSBTIMER_VALUEREG_LOW	TCNT3L
#	define HWCLOCK_LSBTIMER_VALUEREG_HIGH	TCNT3H

#if 0
#	define CLOCKTIMER_PRESCALER_CODE	(_BV(CS31)|_BV(CS30))
#	define CLOCKTIMER_PRESCALER_VALUE	(64ULL)
#else
#	define CLOCKTIMER_PRESCALER_CODE	(_BV(CS32)|_BV(CS30))
#	define CLOCKTIMER_PRESCALER_VALUE	(1024ULL)
#endif

/* usually nothing more to do here - clocktimer.h should do the generic stuff */



#endif