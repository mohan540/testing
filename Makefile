CONTIKI_PROJECT = tx
all: $(CONTIKI_PROJECT)

flash: 
	msp430-objcopy $(CONTIKI_PROJECT).cmote -O ihex $(CONTIKI_PROJECT).ihex
	cmote-bsl --cmote -c /dev/ttyUSB0 -r -I -p $(CONTIKI_PROJECT).ihex

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include
