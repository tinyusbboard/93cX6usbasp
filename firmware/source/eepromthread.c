/*
 * eepromthread.c
 */
#define __EEPROMTHREAD_C_dc83edef7fb74d0f88488010fe346ac7	1

#include "defines.h"
#include "eepromthread.h"
#include "API/tinyusbboard.h"
#include "API/iocomfort.h"
#include "extfunc.h"
#include "cpucontext.h"
#include "hwclock.h"

// #include "avr-threadlib/avr-threads/src/avr-thread.h" /* scheduled non-preemtive */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* http://nongnu.org/avr-libc/user-manual/modules.html */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <util/delay.h>


static int8_t		__eethread_initcounter = 0;
#if   (RAMEND < 0x400)
/* less than 1KiB of RAM is not enough */
#	error Not enough SRAM on device for using this firmware
#elif (RAMEND < 0x460) 
/* 1KiB of RAM is just enough */
static uint8_t		__eethread_stack[128];
#else
/* more than 1KiB of RAM (most likely at least 2 KiB) */
static uint8_t		__eethread_stack[256]; /* be a bit more generous */
#endif

/* only to be called from within the eethread context !! */
#define __eethread_yield()	__eethread_sleep(0)
EXTFUNCFAR static void __eethread_sleep(uint16_t ticks) {
  _eethread_sleepTicks	= ticks;
  _eethread_lastYield	= EXTFUNC_callByName(hwclock_now);
#if 0
  while (1) {
    hwclock_time_t thenow, thedelta;
    EXTFUNC_callByName(cpucontext_switch, _eethread_switchback);
    thenow	= EXTFUNC_callByName(hwclock_now);
    thedelta	= EXTFUNC_callByName(hwclock_delta, _eethread_lastYield, thenow);
    if (EXTFUNC_callByName(hwclock_ispassed, thedelta, ticks)) {
      break;
    }
  }
#else
  /* completely rely on operator for time to pass */
  EXTFUNC_callByName(cpucontext_switch, _eethread_switchback);
#endif
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* the actual eeprom  access (preferably called within the eethread-context */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#if (EXTEE_TRYSPIHW > 0)
#	if (0)
#	elif defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__) || defined (__AVR_ATmega8HVA__)
#		include "hw/hwspim8.h"
#		include "hw/hwspim8.c"

#	elif defined (__AVR_ATmega88__)  || defined (__AVR_ATmega88A__)  || defined (__AVR_ATmega88PA__)  || defined (__AVR_ATmega88P__)  || \
	     defined (__AVR_ATmega168__) || defined (__AVR_ATmega168A__) || defined (__AVR_ATmega168PA__) || defined (__AVR_ATmega168P__) || \
	     defined (__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
/* these modells currently can reuse driver-code from atmega8 */
#		include "hw/hwspim8.h"
#		include "hw/hwspim8.c"

#	else
#		warning hardware SPI for the selected AVR modell is not implemented, yet!
#		include "hw/softspi.h"
#		include "hw/softspi.c"
#	endif
#else
#	include "hw/softspi.h"
#	include "hw/softspi.c"
#endif

EXTFUNCFAR void EE_DUMP(uint8_t *storage_buffer) {
  uint16_t i;
  for (i=0; i<EEPROMTHREAD__EE_TOTALBYTES; i++) {
    EE_TXon();
    EE_pulse((EEPROMTHREAD__EECMD_READ), (i));
    storage_buffer[i]=EE_data(0x00);
#if (EE_extraWBTime_us  >  0)
    __eethread_sleep(EE_clockQuarter_us);
#endif
    EE_TXoff();
#if (EE_extraWBTime_us  >  0)
    __eethread_sleep(EE_clockQuarter_us);
#endif
    TOGGLE(LED_ACTIVITY);
  }
  SET_HIGH(LED_ACTIVITY);
}

EXTFUNCFAR void EE_FILL(uint8_t *storage_buffer) {
  uint16_t i;

  /* enable writemode */
  EE_TXon();
  EE_pulse(EEPROMTHREAD__EECMD_EWEN, 0);
  EE_TXoff();
#if (EE_extraWBTime_us  >  0)
    __eethread_sleep(EE_clockQuarter_us);
#endif


  /* erase entire eeprom */
  EE_TXon();
  EE_pulse(EEPROMTHREAD__EECMD_ERAL, 0);
  EE_TXoff();
#if (EE_extraWBTime_us  >  0)
    __eethread_sleep(EE_clockQuarter_us);
#endif

  /* wait for erase to complete ... */
  EE_TXon();
  EE_wait();
  EE_TXoff();
#if (EE_extraWBTime_us  >  0)
    __eethread_sleep(EE_clockQuarter_us);
#endif


  for (i=0; i<EEPROMTHREAD__EE_TOTALBYTES; i++) {
    if (storage_buffer[i] != 0xff) {
      EE_TXon();
      EE_pulse((EEPROMTHREAD__EECMD_WRITE), (i));
      EE_data(storage_buffer[i]);
      EE_TXoff();
#if (EE_extraWBTime_us  >  0)
    __eethread_sleep(EE_clockQuarter_us);
#endif

      EE_TXon();
      EE_wait();
      EE_TXoff();
    }
#if (EE_extraWBTime_us  >  0)
    __eethread_sleep(EE_clockQuarter_us);
#endif
    TOGGLE(LED_ACTIVITY);
  }

  /* disable writemode */
  EE_TXon();
  EE_pulse(EEPROMTHREAD__EECMD_EWDS, 0);
  EE_TXoff();
#if (EE_extraWBTime_us  >  0)
    __eethread_sleep(EE_clockQuarter_us);
#endif

  SET_HIGH(LED_ACTIVITY);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



EXTFUNC(int8_t, __eethread_ctx_init, void* parameters)  {
  /* in order to initialize the context properly, do start it once */
  EE_init();
  _eethread_lastYield	= EXTFUNC_callByName(hwclock_now);
  _eethread_sleepTicks	= 0;
  return 0;
}

EXTFUNC_void(int8_t, eethread_initialize) {
  if (__eethread_initcounter==0) {
    __eethread_initcounter++;
    EXTFUNC_callByName(cpucontext_initialize);
    EXTFUNC_callByName(hwclock_initialize);

    _eethread_switchback=cpucontext_current;
    EXTFUNC_callByName(cpucontext_create, &_eethread_context, &__eethread_stack[0], sizeof(__eethread_stack), EXTFUNC_getPtr(__eethread_ctx_init, CPUCONTEXT_entry_t), NULL);
    do {
      EXTFUNC_callByName(cpucontext_switch, &_eethread_context);
    } while (!(CPUCONTEXT_isFinal(&_eethread_context)));

    return 0;
  }
  return 1;
}
EXTFUNC_void(int8_t, eethread_finalize) {
  if (__eethread_initcounter > 0) {
    __eethread_initcounter--;
    if (__eethread_initcounter==0) {
      EE_final();
      EXTFUNC_callByName(hwclock_finalize);
      EXTFUNC_callByName(cpucontext_finalize);
      return 0;
    }
    return 1;
  }
  return -1;
}





EXTFUNC(int8_t, eethread_operate,hwclock_time_t thenow) {
  if (eethread_isBusy()) {
    _eethread_switchback=cpucontext_current;
    if (_eethread_sleepTicks > 0) {
      hwclock_time_t thedelta = EXTFUNC_callByName(hwclock_delta, _eethread_lastYield, thenow);
      if (EXTFUNC_callByName(hwclock_ispassed, thedelta, _eethread_sleepTicks)) {
	EXTFUNC_callByName(cpucontext_switch, &_eethread_context);
	return 1;
      }
      return -1;
    } else {
      EXTFUNC_callByName(cpucontext_switch, &_eethread_context);
      return 1;
    }
  }
  return 0;
}





EXTFUNC(int8_t, __eethread_debug4ever, void* parameters)  {
#if 1
  while (1) {
    __eethread_sleep(HWCLOCK_UStoTICK(300000));
    TOGGLE(LED_ACTIVITY);
  }
#else
  /* really just for debugging purpose: utilized hardware SPI */
  while (1) {
    SPDR;
    SPDR=0xa5;
    while (!(SPSR & _BV(SPIF))) __eethread_yield();
  }
#endif
  return 0;
}
EXTFUNC_void(int8_t, eethread_debug4ever) {
  if (eethread_finished()) {
    _eethread_sleepTicks	= 0;
    EXTFUNC_callByName(cpucontext_create, &_eethread_context, &__eethread_stack[0], sizeof(__eethread_stack), EXTFUNC_getPtr(__eethread_debug4ever, CPUCONTEXT_entry_t), NULL);
    return 0;
  } else {
    return -1;
  }
  return 1;
}



EXTFUNC(int8_t, __eethread_eedump, void* parameters)  {
  EE_DUMP(parameters);
#if (EE_extraWBTime_us  >  0)
    __eethread_sleep(EE_clockQuarter_us);
#endif

  return 0;
}
EXTFUNC(int8_t, eethread_eedump, void* buffer) {
  if (eethread_finished()) {
    _eethread_sleepTicks	= 0;
    EXTFUNC_callByName(cpucontext_create, &_eethread_context, &__eethread_stack[0], sizeof(__eethread_stack), EXTFUNC_getPtr(__eethread_eedump, CPUCONTEXT_entry_t), buffer);
    return 0;
  } else {
    return -1;
  }
  return 1;
}


EXTFUNC(int8_t, __eethread_eefullwrite, void* parameters)  {
  EE_FILL(parameters);
#if (EE_extraWBTime_us  >  0)
    __eethread_sleep(EE_clockQuarter_us);
#endif
  
  EE_DUMP(parameters);
#if (EE_extraWBTime_us  >  0)
    __eethread_sleep(EE_clockQuarter_us);
#endif

  return 0;
}
EXTFUNC(int8_t, eethread_eefullwrite, void* buffer) {
  if (eethread_finished()) {
    _eethread_sleepTicks	= 0;
    EXTFUNC_callByName(cpucontext_create, &_eethread_context, &__eethread_stack[0], sizeof(__eethread_stack), EXTFUNC_getPtr(__eethread_eefullwrite, CPUCONTEXT_entry_t), buffer);
    return 0;
  } else {
    return -1;
  }
  return 1;
}