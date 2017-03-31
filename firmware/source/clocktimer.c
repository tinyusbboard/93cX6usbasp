/*
 * clocktimer.c
 */
#define __CLOCKTIMER_C_08bc8b223b334fccb47fc2544daa8f80	1

#include "clocktimer.h"
#include "extfunc.h"

EXTFUNC_void(int8_t, clocktimer_initialize) {
  /* just a dummy - nothing to initialize here */
  return 0;
}
EXTFUNC_void(int8_t, clocktimer_finalize) {
  /* just a dummy - nothing to finalize here */
  return 0;
}

/* no more code here - just a wrapper for the actual hardware driver */

