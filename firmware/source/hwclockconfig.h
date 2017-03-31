/*
 * HWCLOCKCONFIG.H
 *
 * Stephan Baerwolf (matrixstorm@gmx.de), Schwansee 2017
 * (please contact me at least before commercial use)
 */

#define HWCLOCK_LSBTIMER_BITS		16
#define HWCLOCK_MSBTIMER_BITS		 0 /* can be 0 if no second timer is used to increase time-window */

#include "clocktimer.h"
