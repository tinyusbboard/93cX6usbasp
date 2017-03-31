/*
 * clocktimer.h
 */
#ifndef __CLOCKTIMER_H_08bc8b223b334fccb47fc2544daa8f80
#define __CLOCKTIMER_H_08bc8b223b334fccb47fc2544daa8f80	1

#include "defines.h"
#include "extfunc.h"

#ifdef __CLOCKTIMER_C_08bc8b223b334fccb47fc2544daa8f80
#	define	CLOCKTIMERPUBLIC
#else
#	define	CLOCKTIMERPUBLIC		extern
#endif



//CODE COMES HERE:
#if (0)
#elif defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__) || defined (__AVR_ATmega8HVA__)

#	include "hw/ctm8.h"
#	ifdef __CLOCKTIMER_C_08bc8b223b334fccb47fc2544daa8f80
#		include "hw/ctm8.c"
#	endif

#elif defined (__AVR_ATmega88__)  || defined (__AVR_ATmega88A__)  || defined (__AVR_ATmega88PA__)  || defined (__AVR_ATmega88P__)  || \
      defined (__AVR_ATmega168__) || defined (__AVR_ATmega168A__) || defined (__AVR_ATmega168PA__) || defined (__AVR_ATmega168P__) || \
      defined (__AVR_ATmega328__) || defined (__AVR_ATmega328P__)

#	include "hw/ctm328.h"
#	ifdef __CLOCKTIMER_C_08bc8b223b334fccb47fc2544daa8f80
#		include "hw/ctm328.c"
#	endif

#elif defined (__AVR_ATmega103__) || defined (__AVR_ATmega128__) || defined (__AVR_ATmega128A__)

#	include "hw/ctm128.h"
#	ifdef __CLOCKTIMER_C_08bc8b223b334fccb47fc2544daa8f80
#		include "hw/ctm128.c"
#	endif

#elif defined (__AVR_ATmega1284__) || defined (__AVR_ATmega1284P__)

#	include "hw/ctm1284p.h"
#	ifdef __CLOCKTIMER_C_08bc8b223b334fccb47fc2544daa8f80
#		include "hw/ctm1284p.c"
#	endif

#elif defined (__AVR_ATmega640__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega1281__) || defined (__AVR_ATmega2560__) || defined (__AVR_ATmega2561__)

#	include "hw/ctm2560.h"
#	ifdef __CLOCKTIMER_C_08bc8b223b334fccb47fc2544daa8f80
#		include "hw/ctm2560.c"
#	endif

#else

#	define HWCLOCK_LSBTIMER_VALUEREG_LOW	SREG /* use this dummy register, since it is available on all AVRs */
#	define HWCLOCK_LSBTIMER_VALUEREG_HIGH	SREG /* use this dummy register, since it is available on all AVRs */

#	error unsupported AVR controller
#endif



/* if the hardware driver doesn't specify it use the generic slow way */
#ifndef __HWCLOCK_NSPERTICK
#	ifdef CLOCKTIMER_PRESCALER_VALUE
#		define __HWCLOCK_NSPERTICK		(((CLOCKTIMER_PRESCALER_VALUE) * (1000000000ULL)) / (F_CPU))
#	else
#		warning - can not compute nanoseconds per tick, since clocktime-drive does not specify prescaler
#	endif
#endif

#ifndef HWCLOCK_LSBTIMER_USEIOACCESS
#	define HWCLOCK_LSBTIMER_USEIOACCESS	 0 /* =1 iff registers can be accessed faster via IO-memory region */
#endif

#ifndef HWCLOCK_MSBTIMER_USEIOACCESS
#	define HWCLOCK_MSBTIMER_USEIOACCESS	 0 /* =1 iff registers can be accessed faster via IO-memory region */
#endif


CLOCKTIMERPUBLIC EXTFUNC_voidhead(int8_t, clocktimer_initialize);
CLOCKTIMERPUBLIC EXTFUNC_voidhead(int8_t, clocktimer_finalize);

#endif
