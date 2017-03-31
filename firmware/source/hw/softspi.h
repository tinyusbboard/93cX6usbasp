/*
 * softspi.h
 * 
 * generic software spi
 * 
 */
#ifndef __SOFTSPI_H_8ee27e4be0ba442fa36c02734b9a0023
#define __SOFTSPI_H_8ee27e4be0ba442fa36c02734b9a0023	1


/* usually nothing more to do here - clocktimer.h should do the generic stuff */

#define EE_init		SOFTSPI_EE_init
#define EE_final	SOFTSPI_EE_final
#define EE_TXon		SOFTSPI_EE_TXon
#define EE_TXoff	SOFTSPI_EE_TXoff

#define EE_pulse	SOFTSPI_EE_pulse
#define EE_data		SOFTSPI_EE_data
#define EE_wait		SOFTSPI_EE_wait

#endif