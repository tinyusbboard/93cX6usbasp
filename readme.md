93cX6usbasp
===========

93cX6usbasp is a project (primary developted for tinyUSBboard, but also intended for other AVR boards) implementing a programmer for the famous 93cX6 eeprom.

Meaning 93cX6usbasp will turn an AVR into an USB enabled device for programming 93c46, 93c56 or 93c66 compatible eeproms from various vendors (ATMEL, Microchip, STM ...).
Therefore 93cX6usbasp emulates an usbasp programmer (http://www.fischl.de/usbasp/) running on ATmega8.
(The firmware does not need to actually run on atmega8 - it is just pretending to run on atmega8 from usbasp perspektive)

This way no new programming software o drivers are needed for the computer. All usbasp tools (avrdude, khazama ...etc...) can be reused.
The only difference is, it is accessing the externally connected 93cX6 instead the internal's eeprom of the (pretended) ATmega8.
For the default layout (defines.h), just connect the following pins (93cX6 <--> AVR) together:


* CS <--> PB2 (SS)
* DI <--> PB3 (MOSI)
* DO <--> PB4 (MISO)
* SK <--> PB5 (SCK)

All configuration can be done via environment variables within the make-system.


The eeproms organisation should be configured to 8-bit.
So in case some problems occure, please check if:

* VCC is connected to the same voltage as AVR
* ORG is pulled to GND
* a 10k pulldown resistor is placed between CS and GND
* a small capacitor is placed between VCC and GND

93cX6usbasp implements hardwareSPI access where it is applicable and falls back on softwareSPI when neccessary.
Because of the way how 93cX6usbasp works internally (asynchron access to and cache of external eeprom, since softwareSPI might be slow): Please DO NOT RELY on verification checks right after any writes.
Verification should be done after at least one disconnect to the programmer. (Caches are flushed and resynced after usbasp-disconnects.)


93cX6usbasp also serves as an example of cooperative multitasking via virtual cpu-contexts on AVRs together with USB.
It can (and should!) be modified also to programm other memories, too.

by Stephan Baerwolf, Schwansee 2017
