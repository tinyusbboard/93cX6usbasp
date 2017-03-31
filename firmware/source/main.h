/*
 * main.h
 */
#ifndef __MAIN_H_dc83edef7fb74d0f88488010fe346ac7
#define __MAIN_H_dc83edef7fb74d0f88488010fe346ac7	1

#include "defines.h"
#include "extfunc.h"
#include "hwclock.h"

#include <avr/io.h>

#ifdef __MAIN_C_dc83edef7fb74d0f88488010fe346ac7
#	define	MAINPUBLIC
#else
#	define	MAINPUBLIC		extern
#endif


#ifdef MCUSR
#	define	MCUBOOTREASONREG	MCUSR
#else
#	define	MCUBOOTREASONREG	MCUCSR
#endif
MAINPUBLIC static uint8_t bootupreason;

//YOUR CODE HERE:
// MAINPUBLIC static EXTFUNC_functype(hwclock_waitcallback_t) main_spinwait_func = EXTFUNC_NULL;

#endif
