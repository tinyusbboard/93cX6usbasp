/*
 * defines.h
 */
#ifndef __DEFINES_H_94b054518dfc475a9d5b532207a60b61
#define __DEFINES_H_94b054518dfc475a9d5b532207a60b61 1

//GLOBAL DEFINES:

#ifndef FIRMWARE_FOR__EEPROMTYPE_93CXX
#	define FIRMWARE_FOR__EEPROMTYPE_93CXX 46
#endif
// #define FIRMWARE_FOR__EEPROMTYPE_93CXX 46
// #define FIRMWARE_FOR__EEPROMTYPE_93CXX 56
// #define FIRMWARE_FOR__EEPROMTYPE_93CXX 66

#ifndef LED_ACTIVITY
#	define LED_ACTIVITY	LED_LEFT
#endif

#ifndef LED_CONNECTED
#	define LED_CONNECTED	LED_B
#endif

#ifndef EXTEE_SK
#	define EXTEE_SK		B,5
#endif

#ifndef EXTEE_CS
#	define EXTEE_CS		B,2
#endif

#ifndef EXTEE_DI
#	define EXTEE_DI		B,3
#endif

#ifndef EXTEE_DO
#	define EXTEE_DO		B,4
#endif

#ifndef EXTEE_TRYSPIHW
#	define EXTEE_TRYSPIHW	1
#endif

#endif
