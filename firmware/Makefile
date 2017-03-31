#######################################################################################
# environment variable of the current user to locate the AVR8 toolchain
AVRPATH = $(AVR8TOOLCHAINBINDIR)

# the type of avr microcontroller
DEVICE = atmega8
MCUARCH= avr4
# DEFINES += -D__AVR_LIBC_DEPRECATED_ENABLE__
EFUSE  = ""
HFUSE  = 0xd9
LFUSE  = 0xe1

# the frequency the microcontroller is clocked with
F_CPU = 16000000

#tinyUSBboard Rev3 and newer
DEFINES += -DVUSB_CFG_IOPORTNAME=D -DVUSB_CFG_DMINUS_BIT=7 -DVUSB_CFG_DPLUS_BIT=2
DEFINES += -DVUSB_CFG_HASNO_PULLUP_IOPORTNAME -DVUSB_CFG_HASNO_PULLUP_BIT
# DEFINES += -DEXTEE_TRYSPIHW=0

#special board example
# DEFINES += -DVUSB_CFG_IOPORTNAME=A -DVUSB_CFG_DMINUS_BIT=4 -DVUSB_CFG_DPLUS_BIT=5
# DEFINES += -DVUSB_CFG_HASNO_PULLUP_IOPORTNAME -DVUSB_CFG_HASNO_PULLUP_BIT
# DEFINES += -DBUTTON_PROG=A,6
# DEFINES += -DEXTEE_DO=D,7 -DEXTEE_DI=C,0 -DEXTEE_SK=C,1 -DEXTEE_CS=C,2
# DEFINES += -DUSB_INTR_CFG=PCMSK0 "-DUSB_INTR_CFG_SET=(1<<PCINT5)" -DUSB_INTR_CFG_CLR=0 -DUSB_INTR_ENABLE=PCICR -DUSB_INTR_ENABLE_BIT=PCIE0 -DUSB_INTR_PENDING=PCIFR -DUSB_INTR_PENDING_BIT=PCIF0 -DVUSB_INTR_VECTOR=PCINT0_vect
# DEFINES += -DUSE_SPMINTEREFACE_MAGICVALUE=0

# extra data section
DEFINES += -DEXTFUNCINCLUDEDEFINES -DCPUCONTEXTINCLUDEDEFINES -DHWCLOCKINCLUDEDEFINES
DEFINES += -DMYDEBUG=3 -DFIRMWARE_FOR__EEPROMTYPE_93CXX=46
# DEFINES += -DEXTFUNC_NOEXT=0
# DEFINES += -DCPUCONTEXT_EXTRASYMBOLS=1
# DEFINES += -DEXTFUNCFAR=__attribute__\ \(\(section\ \(\".farfunc\"\)\)\) -Wl,--section-start=.farfunc=0x1300
# DEFINES += -DEXTFUNCNEAR=PROGMEM
# DEFINES += -DEXTFUNCNEAR=__attribute__\ \(\(section\ \(\".nearfunc\"\)\)\) -Wl,--section-start=.nearfunc=0x1000

# DEFINES += -DDATASECTION=__attribute__\ \(\(section\ \(\".extradata\"\)\)\)
# LDFLAGS += -Wl,--section-start=.extradata=0x6000

# where the firmware should be located within the flashmemory (in case you trampoline)
FLASHADDRESS = 0x0000

# (not important for compiling) - the device transporting firmware into the controller
PROGRAMMER = -c usbasp

#######################################################################################



# Tools:
ECHO=@echo
GCC=gcc
RM=@rm -f

DOX=@doxygen

CC=$(AVRPATH)avr-gcc
OBC=@$(AVRPATH)avr-objcopy
OBD=@$(AVRPATH)avr-objdump
SIZ=@$(AVRPATH)avr-size

AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)
AVRDUDE_FUSE = -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m
ifneq ($(EFUSE), "")
AVRDUDE_FUSE += -U efuse:w:$(EFUSE):m
endif


MYCFLAGS = -Wall -g3 -ggdb -Os -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions -ffunction-sections -fdata-sections -I. -Isource -Ilibraries -Ilibraries/avr-contexts/include -Ilibraries/v-usb/usbdrv -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) $(CFLAGS)   $(DEFINES)
MYLDFLAGS = -Wl,--relax,--gc-sections $(LDFLAGS)


FLASHPREAMBLEDEFINE = 
ifneq ($(FLASHADDRESS), 0)
ifneq ($(FLASHADDRESS), 00)
ifneq ($(FLASHADDRESS), 000)
ifneq ($(FLASHADDRESS), 0000)
ifneq ($(FLASHADDRESS), 00000)
ifneq ($(FLASHADDRESS), 0x0)
ifneq ($(FLASHADDRESS), 0x00)
ifneq ($(FLASHADDRESS), 0x000)
ifneq ($(FLASHADDRESS), 0x0000)
ifneq ($(FLASHADDRESS), 0x00000)
FLASHPREAMBLE = 0x0000
FLASHPREAMBLEDEFINE = -DFLASHPREAMBLE=$(FLASHPREAMBLE)
MYLDFLAGS += -Wl,--section-start=.text=$(FLASHADDRESS)
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif




STDDEP	 = *.h source/*.h source/hw/* libraries/API/*.h libraries/avr-contexts/include/*
EXTRADEP = Makefile


all: release/main.hex release/eeprom.hex release/main.bin release/eeprom.bin release/main.asm build/main.asm


build/extfunc.o: libraries/avr-contexts/source/extfunc.c $(STDDEP) $(EXTRADEP) 
	$(CC) libraries/avr-contexts/source/extfunc.c -c -o build/extfunc.o $(MYCFLAGS)
build/cpucontext.o: libraries/avr-contexts/source/cpucontext.c $(STDDEP) $(EXTRADEP)
	$(CC) libraries/avr-contexts/source/cpucontext.c -c -o build/cpucontext.o $(MYCFLAGS)
build/hwclock.o: libraries/avr-contexts/source/hwclock.c $(STDDEP) $(EXTRADEP)
	$(CC) libraries/avr-contexts/source/hwclock.c -c -o build/hwclock.o $(MYCFLAGS)


build/usbdrvasm.o: libraries/v-usb/usbdrv/usbdrvasm.S $(STDDEP) $(EXTRADEP)
	$(CC) -x assembler-with-cpp -c libraries/v-usb/usbdrv/usbdrvasm.S -o build/usbdrvasm.o $(MYCFLAGS)

build/oddebug.o: libraries/v-usb/usbdrv/oddebug.c $(STDDEP) $(EXTRADEP)
	$(CC) libraries/v-usb/usbdrv/oddebug.c -c -o build/oddebug.o $(MYCFLAGS)

build/usbdrv.o: libraries/v-usb/usbdrv/usbdrv.c $(STDDEP) $(EXTRADEP)
	$(CC) libraries/v-usb/usbdrv/usbdrv.c -c -o build/usbdrv.o $(MYCFLAGS)



build/clocktimer.o: source/clocktimer.c $(STDDEP) $(EXTRADEP)
	$(CC) source/clocktimer.c -c -o build/clocktimer.o $(MYCFLAGS)

build/eepromthread.o: source/eepromthread.c $(STDDEP) $(EXTRADEP)
	$(CC) source/eepromthread.c -c -o build/eepromthread.o $(MYCFLAGS)

build/main.o: source/main.c $(STDDEP) $(EXTRADEP)
	$(CC) source/main.c -c -o build/main.o $(MYCFLAGS)

build/apipage.o: libraries/API/apipage.c $(STDDEP) $(EXTRADEP)
	$(CC) libraries/API/apipage.c -c -o build/apipage.o $(MYCFLAGS)




MYOBJECTS = build/main.o build/clocktimer.o build/eepromthread.o build/apipage.o build/extfunc.o build/cpucontext.o build/hwclock.o build/usbdrvasm.o build/oddebug.o build/usbdrv.o
release/main.elf: $(MYOBJECTS) $(STDDEP) $(EXTRADEP)
	$(CC) $(MYOBJECTS) -o release/main.elf $(MYCFLAGS) -Wl,-Map,release/main.map $(MYLDFLAGS)
	$(ECHO) "."
	$(SIZ) release/main.elf
	$(ECHO) "."

release/main.asm: release/main.elf $(STDDEP) $(EXTRADEP)
	$(OBD) -d release/main.elf > release/main.asm

build/main.asm: release/main.elf $(STDDEP) $(EXTRADEP)
	$(OBD) -dS release/main.elf > build/main.asm

release/main.hex: release/main.elf $(STDDEP) $(EXTRADEP)
	$(OBC) -R .eeprom -R .fuse -R .lock -R .signature -O ihex release/main.elf release/main.hex

release/eeprom.hex: release/main.elf $(STDDEP) $(EXTRADEP)
	$(OBC) -j .eeprom -O ihex release/main.elf release/eeprom.hex

release/main.bin: release/main.elf $(STDDEP) $(EXTRADEP)
	$(OBC) -R .eeprom -R .fuse -R .lock -R .signature -O binary release/main.elf release/main.bin

release/eeprom.bin: release/main.elf $(STDDEP) $(EXTRADEP)
	$(OBC) -j .eeprom -O binary release/main.elf release/eeprom.bin

disasm: release/main.elf $(STDDEP) $(EXTRADEP)
	$(OBD) -d release/main.elf

fuse:
	$(ECHO) "."
	$(AVRDUDE) $(AVRDUDE_FUSE)
	$(ECHO) "."

libraries/USBaspTool/usbasploader:
	cd libraries/USBaspTool; $(MAKE)

exit:  libraries/USBaspTool/usbasploader
	libraries/USBaspTool/usbasploader exit

flash: all
	$(ECHO) "."
	$(AVRDUDE) -U flash:w:release/main.hex:i
	$(ECHO) "."

eeprom: all
	$(ECHO) "."
	$(AVRDUDE) -D -U eeprom:w:release/eeprom.hex:i
	$(ECHO) "."

deepclean: clean
	$(RM) source/*~
	$(RM) *~

clean:
	$(RM) build/*
	$(RM) release/*
	cd libraries/USBaspTool; $(MAKE) clean