/*
 * eepromthread.h
 */
#ifndef __EEPROMTHREAD_H_dc83edef7fb74d0f88488010fe346ac7
#define __EEPROMTHREAD_H_dc83edef7fb74d0f88488010fe346ac7	1

#include "defines.h"
#include "extfunc.h"
#include "cpucontext.h"
#include "hwclock.h"

#ifdef __EEPROMTHREAD_C_dc83edef7fb74d0f88488010fe346ac7
#	define	EEPROMTHREADPUBLIC
#else
#	define	EEPROMTHREADPUBLIC		extern
#endif

// #define EE_clockQuarter_us	HWCLOCK_UStoTICK(1000)
#define EE_clockQuarter_us	HWCLOCK_UStoTICK(200)
// #define EE_clockQuarter_us	HWCLOCK_UStoTICK(50)

// #define EE_extraWBTime_us	(EE_clockQuarter_us)
#define EE_extraWBTime_us	(0)


EEPROMTHREADPUBLIC hwclock_time_t	_eethread_lastYield;
EEPROMTHREADPUBLIC uint16_t		_eethread_sleepTicks;
EEPROMTHREADPUBLIC cpucontext_t		_eethread_context;
EEPROMTHREADPUBLIC cpucontext_t*	_eethread_switchback;

EEPROMTHREADPUBLIC EXTFUNC_voidhead(int8_t, eethread_initialize);
EEPROMTHREADPUBLIC EXTFUNC_voidhead(int8_t, eethread_finalize);

/* check and process a bit of work, then and switch back to caller */
EEPROMTHREADPUBLIC EXTFUNC_head(int8_t, eethread_operate, hwclock_time_t thenow);


#if  ( ((FIRMWARE_FOR__EEPROMTYPE_93CXX)==66) || ((FIRMWARE_FOR__EEPROMTYPE_93CXX)==56) )

#	define EEPROMTHREAD__EE_INITIALPULSESHIFT	  (4)
#	define EEPROMTHREAD__EE_CMDPULSEBITS		 (12)

#	if  ((FIRMWARE_FOR__EEPROMTYPE_93CXX)==56)
#		define EEPROMTHREAD__EE_TOTALBYTES	(256)
#	else
#		define EEPROMTHREAD__EE_TOTALBYTES	(512)
#	endif

#	define EEPROMTHREAD__EECMD_READ			(0b110000000000)
#	define EEPROMTHREAD__EECMD_EWEN			(0b100110000000)
#	define EEPROMTHREAD__EECMD_ERASE		(0b111000000000)
#	define EEPROMTHREAD__EECMD_WRITE		(0b101000000000)
#	define EEPROMTHREAD__EECMD_ERAL			(0b100100000000)
#	define EEPROMTHREAD__EECMD_WRAL			(0b100010000000)
#	define EEPROMTHREAD__EECMD_EWDS			(0b100000000000)


#else


#	if ((FIRMWARE_FOR__EEPROMTYPE_93CXX)!=46)
#		warning  unknown EEPROM - assuming 93C46 here! 
#	endif

#	define EEPROMTHREAD__EE_INITIALPULSESHIFT	  (6)
#	define EEPROMTHREAD__EE_CMDPULSEBITS		 (10)
#	define EEPROMTHREAD__EE_TOTALBYTES		(128)

#	define EEPROMTHREAD__EECMD_READ			(0b1100000000)
#	define EEPROMTHREAD__EECMD_EWEN			(0b1001100000)
#	define EEPROMTHREAD__EECMD_ERASE		(0b1110000000)
#	define EEPROMTHREAD__EECMD_WRITE		(0b1010000000)
#	define EEPROMTHREAD__EECMD_ERAL			(0b1001000000)
#	define EEPROMTHREAD__EECMD_WRAL			(0b1000100000)
#	define EEPROMTHREAD__EECMD_EWDS			(0b1000000000)

#endif

/* invoke an infinite task doing sth. debuggable */
EEPROMTHREADPUBLIC EXTFUNC_voidhead(int8_t, eethread_debug4ever);

/* read the entire eeprom to buffer */
EEPROMTHREADPUBLIC EXTFUNC_head(int8_t, eethread_eedump, void *buffer);

/* clear the eeprom and write the entire buffer to eeprom - also dump again */
EEPROMTHREADPUBLIC EXTFUNC_head(int8_t, eethread_eefullwrite, void *buffer);

#define eethread_finished()		(CPUCONTEXT_isFinal(&_eethread_context))
#define eethread_isBusy()		(!(eethread_finished()))

#endif
