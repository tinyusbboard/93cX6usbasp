/*
 * softspi.c
 * 
 * generic software spi
 * 
 * !!    not to be compiled directly    !!
 * !!         see eepromthread.c        !!
 * 
 */
#define __SOFTSPI_C_8ee27e4be0ba442fa36c02734b9a0023	1

EXTFUNCFAR void SOFTSPI_EE_init(void) {
  SET_LOW(EXTEE_SK);
  CFG_OUTPUT(EXTEE_SK);
  
  SET_LOW(EXTEE_DI);
  CFG_OUTPUT(EXTEE_DI);

  SET_LOW(EXTEE_CS);
  CFG_OUTPUT(EXTEE_CS);
  
  SET_LOW(EXTEE_DO);
  CFG_INPUT(EXTEE_DO);
  CFG_PULLUP(EXTEE_DO);
}
EXTFUNCFAR void SOFTSPI_EE_final(void) {
  SET_LOW(EXTEE_SK);
  CFG_INPUT(EXTEE_SK);
  
  SET_LOW(EXTEE_DI);
  CFG_INPUT(EXTEE_DI);

  SET_LOW(EXTEE_CS);
  CFG_INPUT(EXTEE_CS);
  
  SET_LOW(EXTEE_DO);
  CFG_INPUT(EXTEE_DO);
}

EXTFUNCFAR void SOFTSPI_EE_TXon(void) {
  SET_HIGH(EXTEE_CS);
}

EXTFUNCFAR void SOFTSPI_EE_TXoff(void) {
  SET_LOW(EXTEE_CS);
  __eethread_sleep(EE_clockQuarter_us);
  SET_HIGH(EXTEE_SK);
  __eethread_sleep(EE_clockQuarter_us*2);
  SET_LOW(EXTEE_SK);
  __eethread_sleep(EE_clockQuarter_us);
}

EXTFUNCFAR void SOFTSPI_EE_pulse(uint16_t data, uint16_t addr) {
  uint8_t i;
  data|=addr;
  data<<=EEPROMTHREAD__EE_INITIALPULSESHIFT;
  
  for (i=0; i<EEPROMTHREAD__EE_CMDPULSEBITS; i++) {
    if (data & 0x8000)	SET_HIGH(EXTEE_DI);
    else		SET_LOW(EXTEE_DI);
    __eethread_sleep(EE_clockQuarter_us);
    SET_HIGH(EXTEE_SK);
    __eethread_sleep(EE_clockQuarter_us*2);
    SET_LOW(EXTEE_SK);
    __eethread_sleep(EE_clockQuarter_us);
    data<<=1;
  }
}

EXTFUNCFAR uint8_t SOFTSPI_EE_data(uint8_t data) {
  uint8_t i, result=0;
  for (i=0; i<8; i++) {
    result<<=1;
    if (data & 0x80)	SET_HIGH(EXTEE_DI);
    else		SET_LOW(EXTEE_DI);
    __eethread_sleep(EE_clockQuarter_us);
    SET_HIGH(EXTEE_SK);
    __eethread_sleep(EE_clockQuarter_us);
    if (!IS_PRESSED(EXTEE_DO)) result|=1;
    __eethread_sleep(EE_clockQuarter_us);
    SET_LOW(EXTEE_SK);
    __eethread_sleep(EE_clockQuarter_us);
    data<<=1;
  }
  return result;
}

EXTFUNCFAR void SOFTSPI_EE_wait(void) {
  uint8_t result = 0;
  do {
    result<<=1;
    SET_LOW(EXTEE_DI);
    __eethread_sleep(EE_clockQuarter_us);
    SET_HIGH(EXTEE_SK);
    __eethread_sleep(EE_clockQuarter_us);
    if (!IS_PRESSED(EXTEE_DO)) result|=1;
    __eethread_sleep(EE_clockQuarter_us);
    SET_LOW(EXTEE_SK);
    __eethread_sleep(EE_clockQuarter_us);

  } while (result==0);
}
