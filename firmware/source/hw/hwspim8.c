/*
 * hwspim8.c
 * 
 * hardware spi implementation for atmega8
 * 
 * !!    not to be compiled directly    !!
 * !!         see eepromthread.c        !!
 * 
 * Enable the atmega8's only SPI to do the
 * EE communication,
 * Also implement a proper 8-bit aligning.
 * 
 * Use the SPI in MASTER with MSB FIRST and
 * low clock polarity (CPOL=0) and
 * low clock phase (CPHA=0).
 * This means our data (value of DI) is placed
 * before rising edge (where the EE is sampling it)
 * and DO is sampled before falling edge.(where the
 * EE is setting it).
 * 
 * DO NOT USE interrupts!
 *    THE SPI IS ASSUMED TO BE EXCLUSIVE
 * 
 */
#define __HWSPIM8_C_8ee27e4be0ba442fa36c02734b9a0023	1

#include "softspi.c"

/* hardware SPI only works with specific layout */
#if ((EXTEE_CS==(B,2)) && (EXTEE_DI==(B,3)) && (EXTEE_DO==(B,4)) && (EXTEE_SK==(B,5)))

#include <avr/io.h>

// #define __HWSPI_EXTRACAREFUL
// #define __HWSPI_EXTRADELAY(x) __eethread_sleep(EE_clockQuarter_us);
#define __HWSPI_EXTRADELAY(x)

EXTFUNCFAR void HWSPIM8_EE_init(void) {
  SOFTSPI_EE_init();
  SPCR = 0;
  SPDR;
  SPSR = HWSPI_PRESCALER_CODE2;
  SPCR = _BV(SPE) | _BV(MSTR) | 0*_BV(CPOL) | 0*_BV(CPHA) | (HWSPI_PRESCALER_CODE1);
}
EXTFUNCFAR void HWSPIM8_EE_final(void) {
  SPCR = 0;
  SPSR = 0;
  SOFTSPI_EE_final();
}

EXTFUNCFAR static void __HWSPIM8_slowdown(uint8_t j) {
  uint8_t i;
  for (i=0;i<j;i++) {
    /* 
    * usually wait one clock - but HWSPI is soo much faster: 
    * simply do not care - clock 8bit "0" out just to have SK moving...
    */
    SPDR;
    __HWSPI_EXTRADELAY();
    SPDR=0;
    while (!(SPSR & _BV(SPIF))) __eethread_yield();
  }
}

EXTFUNCFAR void HWSPIM8_EE_TXon(void) {
  SET_HIGH(EXTEE_CS);
  __HWSPI_EXTRADELAY();
  __HWSPIM8_slowdown(0);
}

EXTFUNCFAR void HWSPIM8_EE_TXoff(void) {
  SET_LOW(EXTEE_CS);
  __HWSPI_EXTRADELAY();
  __HWSPIM8_slowdown(1);
}

EXTFUNCFAR void HWSPIM8_EE_pulse(uint16_t data, uint16_t addr) {
#ifndef __HWSPI_EXTRACAREFUL
  /* ** work around the stalling bit problem within read cmd** */
  /* we have some bits leftover in the current cmd specification anyway */
  if (data==EEPROMTHREAD__EECMD_READ)	data=(data|addr)<<1;
  else					data=data|addr;
  /* when the last bit of data is send, the virtual data bit is sampled */
  /* this avoids sampling of the virtual bit during followed receive */
#else
  data |= addr;
#endif
  SPDR;
  __HWSPI_EXTRADELAY();
  SPDR=(data >> 8);
  while (!(SPSR & _BV(SPIF))) __eethread_yield();
  SPDR;
  __HWSPI_EXTRADELAY();
  SPDR=(data & 0xff);
  while (!(SPSR & _BV(SPIF))) __eethread_yield();
}

EXTFUNCFAR uint8_t HWSPIM8_EE_data(uint8_t data) {
#ifndef __HWSPI_EXTRACAREFUL
  SPDR;
  __HWSPI_EXTRADELAY();
  SPDR=data;
  while (!(SPSR & _BV(SPIF))) __eethread_yield();
  return SPDR;
#else
  uint16_t result;
  SPDR;
  __HWSPI_EXTRADELAY();
  SPDR=data;
  while (!(SPSR & _BV(SPIF))) __eethread_yield();
  result=SPDR;
  SPDR=0;
  result<<=8;
  while (!(SPSR & _BV(SPIF))) __eethread_yield();
  result|=SPDR;
  result>>=7;
  return (result & 0xff);
#endif
}

EXTFUNCFAR void HWSPIM8_EE_wait(void) {
  SPDR;
  do {
    SPDR=0;
    __HWSPI_EXTRADELAY();
    while (!(SPSR & _BV(SPIF))) __eethread_yield();
  } while (SPDR == 0);
}

#else
#	warning generic pinout does not support hardware SPI
#	define HWSPIM8_EE_init	SOFTSPI_EE_init
#	define HWSPIM8_EE_final	SOFTSPI_EE_final

#	define HWSPIM8_EE_TXon	SOFTSPI_EE_TXon
#	define HWSPIM8_EE_TXoff	SOFTSPI_EE_TXoff

#	define HWSPIM8_EE_pulse	SOFTSPI_EE_pulse
#	define HWSPIM8_EE_data	SOFTSPI_EE_data
#	define HWSPIM8_EE_wait	SOFTSPI_EE_wait

#endif