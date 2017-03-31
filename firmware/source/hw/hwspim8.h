/*
 * hwspim8.h
 * 
 * generic software spi
 * 
 */
#ifndef __HWSPIM8_H_8ee27e4be0ba442fa36c02734b9a0023
#define __HWSPIM8_H_8ee27e4be0ba442fa36c02734b9a0023	1


#include "softspi.h"

/* EEPROMs should be able to do 2 MHz SK - 
 *but do slowest SPI here */
#define HWSPI_PRESCALER_CODE1	(_BV(SPR1) | _BV(SPR0))
#define HWSPI_PRESCALER_CODE2	(0*_BV(SPI2X))
#define HWSPI_PRESCALER_VALUE	(128ULL)

#undef EE_init
#define EE_init		HWSPIM8_EE_init

#undef EE_final
#define EE_final	HWSPIM8_EE_final


#undef EE_TXon
#define EE_TXon		HWSPIM8_EE_TXon

#undef EE_TXoff
#define EE_TXoff	HWSPIM8_EE_TXoff


#undef EE_pulse
#define EE_pulse	HWSPIM8_EE_pulse

#undef EE_data
#define EE_data		HWSPIM8_EE_data

#undef EE_wait
#define EE_wait		HWSPIM8_EE_wait

/* usually nothing more to do here - clocktimer.h should do the generic stuff */

#endif