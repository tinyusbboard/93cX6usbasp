/*
 * main.c
 */
#define __MAIN_C_dc83edef7fb74d0f88488010fe346ac7	1

#include "main.h"
#include "eepromthread.h"
#include "API/tinyusbboard.h"
#include "API/iocomfort.h"
#include "API/apipage.h"

#include "extfunc.h"
#include "cpucontext.h"
#include "hwclock.h"

#include "usbdrv.h"

#include "usbasp.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <string.h>


/* http://nongnu.org/avr-libc/user-manual/modules.html */
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/boot.h>

#include <util/delay.h>

void init_cpu(void) {
  cli();
  bootupreason=MCUBOOTREASONREG;
  MCUBOOTREASONREG=0;
  wdt_disable();

  CFG_OUTPUT(LED_CONNECTED);

#	if ((F_CPU) <= 8000000ULL)
  {
    uint16_t clockcalibration;
    /* check if last word of eeprom contains clock calibration data */
    clockcalibration=eeprom_read_word((void*)((E2END)-1));
    if (clockcalibration != 0xffff) {
      /* calibrate the counter */
      OSCCAL=clockcalibration & 0xff;
    }
  }
#	endif
}
void prepare_reboot(void) {
  /* nothing to do on atmega platform */
}

static uint8_t eepromCache[EEPROMTHREAD__EE_TOTALBYTES];

static uchar replyBuffer[8];
static bool prog_address_newmode = false;
static unsigned long prog_address;

static uchar prog_state = PROG_STATE_IDLE;
static bool  eeprom_dirty = true;
static uchar eeprom_state = PROG_STATE_IDLE;

#define MAIN__FIRMWARE_EXIT_SWEXITBIT 0
#define MAIN__FIRMWARE_EXIT_CONECTBIT 7
static uint8_t firmware_exit = _BV(MAIN__FIRMWARE_EXIT_SWEXITBIT);

static unsigned int prog_nbytes = 0;


static const uchar  signatureBytes[4] = {0x1e, 0x93, 0x07, 0};
USB_PUBLIC usbMsgLen_t usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (void *)data;
  usbMsgLen_t len  = 0;
  if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_VENDOR) {

    if (rq->bRequest == USBASP_FUNC_CONNECT) {
      /* connect */
      SET_HIGH(LED_CONNECTED);
      prog_address_newmode = false;
      firmware_exit |= _BV(MAIN__FIRMWARE_EXIT_CONECTBIT);
      memset(replyBuffer, 0, sizeof(replyBuffer));

    } else if (rq->bRequest == USBASP_FUNC_DISCONNECT) {
      /* DISconnect */
      SET_LOW(LED_CONNECTED);
      firmware_exit &= (~(_BV(MAIN__FIRMWARE_EXIT_CONECTBIT)));
      if (eeprom_dirty) {
	if (eethread_finished()) {
	  if (eeprom_state == PROG_STATE_IDLE) {
	    eeprom_state=PROG_STATE_WRITEEEPROM;
	    EXTFUNC_callByName(eethread_eefullwrite, &eepromCache[0]);
	  }
	}
      }

    } else if (rq->bRequest == USBASP_FUNC_ENABLEPROG) {
//    replyBuffer[0]=eethread_isBusy();
      replyBuffer[0]=eeprom_state!=PROG_STATE_IDLE;
      len=1; 

    } else if (rq->bRequest == USBASP_FUNC_SETISPSCK) {
      replyBuffer[0]=0;
      len=1;

    } else if (rq->bRequest == USBASP_FUNC_TRANSMIT) {
	replyBuffer[0]=0;
	replyBuffer[1]=0;
	replyBuffer[2]=0;
	replyBuffer[3]=0;
	len=4;
	if(rq->wValue.bytes[0] == 0x30) { /* read signature */
	  uint8_t h  = rq->wIndex.bytes[0] & 3;
	  replyBuffer[3]=signatureBytes[h];
        } else if((rq->wValue.word == 0xffff) && (rq->wIndex.word == 0xffff)){  /* usbasp software exit usbasptool  */
	  firmware_exit &= (~(_BV(MAIN__FIRMWARE_EXIT_SWEXITBIT)));
        } else if(rq->wValue.bytes[0] == 0xff && rq->wValue.bytes[1] == 0x00){  /* usbasp software exit cmdline */
	  firmware_exit &= (~(_BV(MAIN__FIRMWARE_EXIT_SWEXITBIT)));
#ifdef MYDEBUG
        } else if((rq->wValue.word == 0xffff) && (rq->wIndex.word == 0x7dff)){  /* usbasp firmware specific cmd: reread eeprom content */
	  replyBuffer[0] = eeprom_state;
	  replyBuffer[1] = prog_state;
	  replyBuffer[2] = 0;
	  replyBuffer[3] = 0x80;
	  if (eeprom_state == PROG_STATE_IDLE) {
	    if (eethread_finished()) {
	      eeprom_state = PROG_STATE_READEEPROM;
	      replyBuffer[3] = EXTFUNC_callByName(eethread_eedump, &eepromCache[0]);
	    }
	  }
        } else if((rq->wValue.word == 0xffff) && (rq->wIndex.word == 0x7eff)){  /* usbasp firmware specific cmd: switch off debug  */
	  replyBuffer[0] = eeprom_state;
	  replyBuffer[1] = prog_state;
	  replyBuffer[2] = 0;
	  replyBuffer[3] = eethread_isBusy();
	  if (eeprom_state == PROG_STATE_IDLE) {
	    if (replyBuffer[3]) {
	      /* most likely debug on ? */
	      /* *kill the beast* */
	      /* **ugly hack since API doesn't support killing at the moment** */
	      _eethread_context.flags |= _BV(CPUCONTEXT_FLAGBM_finalized);
	    }
	    SET_HIGH(LED_ACTIVITY);
	  }
        } else if((rq->wValue.word == 0xffff) && (rq->wIndex.word == 0x7fff)){  /* usbasp firmware specific cmd: switch on debug  */
	  replyBuffer[3] = EXTFUNC_callByName(eethread_debug4ever);
#endif
        } else if((rq->wValue.word == 0xffff) && (rq->wIndex.word == 0x80ff)){  /* usbasp firmware specific cmd: read eeprom type  */
	  replyBuffer[0] = 0xAD;
	  replyBuffer[1] = 0x93;
	  replyBuffer[2] = 0xCC;
#if  ((FIRMWARE_FOR__EEPROMTYPE_93CXX)==46)
	  replyBuffer[3] = 0x46;
#elif ((FIRMWARE_FOR__EEPROMTYPE_93CXX)==56)
	  replyBuffer[3] = 0x56;
#elif ((FIRMWARE_FOR__EEPROMTYPE_93CXX)==66)
	  replyBuffer[3] = 0x66;
#else
	  replyBuffer[3] = 0x80;
#endif
        } else if(rq->wValue.bytes[0] == 0x58 && rq->wValue.bytes[1] == 0x00){  /* read lock bits */
	  replyBuffer[3] = 0x3f; // boot_lock_fuse_bits_get(GET_LOCK_BITS);
	} else if(rq->wValue.bytes[0] == 0x50 && rq->wValue.bytes[1] == 0x00){  /* read lfuse bits */
	  replyBuffer[3] = 0x3f; // boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
	}else if(rq->wValue.bytes[0] == 0x58 && rq->wValue.bytes[1] == 0x08){  /* read hfuse bits */
	  replyBuffer[3] = 0xc0; // boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
	}else if(rq->wValue.bytes[0] == 0xa0 && (rq->wValue.bytes[1] & 0xfe) == 0x00){  /* read eeprom cell */
	  uint16_t addr = rq->wValue.bytes[1];
	  replyBuffer[3]=0xff;
	  addr <<= 8;
	  addr += rq->wIndex.bytes[0];
	  if (addr < sizeof(eepromCache)) {
	    replyBuffer[3]=eepromCache[addr];
	  }
	}else if(rq->wValue.bytes[0] == 0xc0 && (rq->wValue.bytes[1] & 0xfe) == 0x00){  /* write eeprom cell */
	  uint16_t addr = rq->wValue.bytes[1];
	  addr <<= 8;
	  addr += rq->wIndex.bytes[0];
	  if (addr < sizeof(eepromCache)) {
	    if (eepromCache[addr] != rq->wIndex.bytes[1]) {
	      eeprom_dirty=true;
	      eepromCache[addr]= rq->wIndex.bytes[1];
	    }
	  }
	} else {
	  /* unknown */
	  replyBuffer[3]=0xff;
	}

    } else if (rq->bRequest == USBASP_FUNC_SETLONGADDRESS) {
      prog_address_newmode = true;
      prog_address         = *((unsigned long*) &data[2]);

    } else if (rq->bRequest == USBASP_FUNC_WRITEFLASH) {
      /* ignore writes to the flash, but receive them... */
      prog_nbytes = (data[7] << 8) | data[6];
      prog_state  = PROG_STATE_WRITEFLASH;
      len=USB_NO_MSG; /* the actual ignoring is done within "usbFunctionWrite" */

    } else if (rq->bRequest == USBASP_FUNC_READFLASH) {
      /* actually read the flash of this device */
      if (!prog_address_newmode) {
	prog_address = (data[3] << 8) | data[2];
      }
      prog_nbytes = (data[7] << 8) | data[6];
      prog_state = PROG_STATE_READFLASH;
      len = USB_NO_MSG; /* the actual reading is done within "usbFunctionRead" */

    } else if (rq->bRequest == USBASP_FUNC_WRITEEEPROM) {
      /* TODO: check against USBasp protocol */
      if (!prog_address_newmode) {
	prog_address = (data[3] << 8) | data[2];
      }
      prog_nbytes = (data[7] << 8) | data[6];
      prog_state  = PROG_STATE_WRITEEEPROM;
      len=USB_NO_MSG; /* the actual ignoring is done within "usbFunctionWrite" */

    } else if (rq->bRequest == USBASP_FUNC_READEEPROM) {
      if (!prog_address_newmode) {
	prog_address = (data[3] << 8) | data[2];
      }

      prog_nbytes = (data[7] << 8) | data[6];
      prog_state = PROG_STATE_READEEPROM;
      len = USB_NO_MSG; /* the actual reading is done within "usbFunctionRead" */


#if 0 /* USBaspTool expects capabilities to respond with 0 bytes - exit via software is more important to us here... */
    } else if (rq->bRequest == USBASP_FUNC_GETCAPABILITIES) {
	replyBuffer[0] = (0*USBASP_CAP_0_TPI);
	replyBuffer[1] = 0;
	replyBuffer[2] = 0;
	replyBuffer[3] = 0;
	len=4;
#endif

    } else {
	/* unknown command */
	replyBuffer[0]=0;
	len=0;
    }
  }
  usbMsgPtr = (usbMsgPtr_t)replyBuffer;
  return len;
}
USB_PUBLIC uchar usbFunctionWrite(uint8_t * data, uchar len) {
  uchar result = 0;  
  if (prog_state == PROG_STATE_WRITEFLASH) {
    if (prog_nbytes > len) {
      prog_nbytes-=len;
    } else {
      prog_nbytes=0;
      prog_state = PROG_STATE_IDLE;
      result=1; /* enough data - not expecting more data */
    }
  } else  if (prog_state == PROG_STATE_WRITEEEPROM) {
    uint8_t i;
    for (i=0;i<len;i++) {
      if (prog_address < sizeof(eepromCache)) {
	if (eepromCache[prog_address] != data[i]) {
	  eeprom_dirty=true;
	  eepromCache[prog_address]=data[i];
	}
      }
      prog_address++;
      prog_nbytes--;
      if (prog_nbytes==0) {
	prog_state = PROG_STATE_IDLE;
	result=1; /* enough data - not expecting more data */
	break;
      }
    }
  } else result=0xff;
  return result; 
}
USB_PUBLIC uchar usbFunctionRead(uchar *data, uchar len) {
  uchar i, result = 0; 
  if (prog_state == PROG_STATE_READFLASH) {
    /* fill packet ISP mode */
    for (i = 0; i < len; i++) {
#if (FLASHEND < 65536)
      data[i] = pgm_read_byte_near((uint16_t)prog_address);
#else
      data[i] = pgm_read_byte_far((uint32_t)prog_address);
#endif
      prog_address++;
    }
    /* last packet? */
    if (len < 8) {
      prog_state = PROG_STATE_IDLE;
    }
    result=len;

  } else if (prog_state == PROG_STATE_READEEPROM) {
    for (i = 0; i < len; i++) {
      if (prog_address < sizeof(eepromCache)) {
	data[i] = eepromCache[prog_address];
      } else {
	data[i] = 0xff; 
      }
      prog_address++;
    }
    /* last packet? */
    if (len < 8) {
      prog_state = PROG_STATE_IDLE;
    }
    result=len;

  } else result=0xff;
  return result;    
}

int main(void) {
  hwclock_time_t end_mainloop_time;

  init_cpu();
  extfunc_initialize();

  usbDeviceDisconnect();

  EXTFUNC_callByName(cpucontext_initialize);
  EXTFUNC_callByName(clocktimer_initialize);
  EXTFUNC_callByName(hwclock_initialize);
  EXTFUNC_callByName(eethread_initialize);
  
  CFG_OUTPUT(LED_ACTIVITY);
  SET_HIGH(LED_ACTIVITY);
  CFG_PULLUP(BUTTON_PROG);

  end_mainloop_time = EXTFUNC_callByName(hwclock_now);
  /* cache (or better: start caching) the EEPROM now */
# if  ((FIRMWARE_FOR__EEPROMTYPE_93CXX) == 0)
  // testmode - use the AVRs eeprom itself
  eeprom_read_block(&eepromCache[0], NULL, sizeof(eepromCache));
#else
  //memset(&eepromCache[0], 0xAA, sizeof(eepromCache));
  eeprom_state=PROG_STATE_READEEPROM;
  EXTFUNC_callByName(eethread_eedump, &eepromCache[0]);
#endif

/* 500ms USB initialization delay */
// _delay_ms(500);
//EXTFUNC_callByName(hwclock_spinwait, HWCLOCK_UStoTICK(500000), main_spinwait_func, NULL);
  while (1) {
    /* instead of spinwait - use the time */
    hwclock_time_t now   = EXTFUNC_callByName(hwclock_now);
    hwclock_time_t delta = EXTFUNC_callByName(hwclock_delta, end_mainloop_time, now);
    if (EXTFUNC_callByName(hwclock_ispassed, delta, HWCLOCK_UStoTICK(500000))) {
      break;
    }
    EXTFUNC_callByName(eethread_operate, now);
  }


  usbDeviceConnect();
  usbInit();
  sei();
  SET_HIGH(LED_ACTIVITY);


  {
    while ( (!(IS_PRESSED(BUTTON_PROG))) && (firmware_exit>0) ) {
      usbPoll();
      { /* multitasking */
	hwclock_time_t now	= EXTFUNC_callByName(hwclock_now);
	if (EXTFUNC_callByName(eethread_operate, now)==0) {
	  if (eeprom_state == PROG_STATE_READEEPROM) eeprom_dirty=false;
	  if (eeprom_state == PROG_STATE_WRITEEEPROM) eeprom_dirty=false;
	  eeprom_state = PROG_STATE_IDLE;
	}
      }
    }
  }


  cli();
  usbDeviceDisconnect();
  end_mainloop_time=EXTFUNC_callByName(hwclock_now);
  
  /* finish possible eeprom task */
  if (eeprom_state == PROG_STATE_WRITEEEPROM) {
    while (eethread_isBusy()) {
      hwclock_time_t now   = EXTFUNC_callByName(hwclock_now);
      hwclock_time_t delta = EXTFUNC_callByName(hwclock_delta, end_mainloop_time, now);
      if (EXTFUNC_callByName(hwclock_ispassed, delta, HWCLOCK_UStoTICK(500000))) {
	firmware_exit=0;
      }
      EXTFUNC_callByName(eethread_operate, now);
    }
  }

  /* finish debounce prog_button */
  hwclock_time_t last = EXTFUNC_callByName(hwclock_now);
  while (firmware_exit!=0) {
    hwclock_time_t now   = EXTFUNC_callByName(hwclock_now);
    hwclock_time_t delta = EXTFUNC_callByName(hwclock_delta, end_mainloop_time, now);
    if (EXTFUNC_callByName(hwclock_ispassed, delta, HWCLOCK_UStoTICK(500000))) {
      firmware_exit=0;
    }

    delta = EXTFUNC_callByName(hwclock_delta, last, now);
    if (EXTFUNC_callByName(hwclock_ispassed, delta, HWCLOCK_UStoTICK(50000))) {
      TOGGLE(LED_ACTIVITY);
      last=now;
    }
  }
  SET_HIGH(LED_ACTIVITY);


  EXTFUNC_callByName(eethread_finalize);
  EXTFUNC_callByName(hwclock_finalize);
  EXTFUNC_callByName(clocktimer_finalize);
  EXTFUNC_callByName(cpucontext_finalize);
  extfunc_finalize();
  prepare_reboot();
  bootloader_startup();
  return 0;
}