include makedefs
OUTFILE=blink

# Include all sub directories  use regex to exlude hidden dirs
INCLUDEDIRS=$(shell find * \( ! -regex '.*/\..*' \) -type d)
INCLUDE=-I.
INCLUDE+=$(addprefix -I, $(INCLUDEDIRS))

CFLAGS+= $(INCLUDE)

CFLAGS+=-D'BAUDRATE=115200'

VPATH=lpc1xxx:libs:core
VPATH+=$(addprefix :, $(INCLUDEDIRS))

OBJFILES = LPC1xxx_startup.o LPC13xx_handlers.o core_cm3.o system_LPC13xx.o main.o uart.o h2rgb.o pwm.o filt_adc.o
#OBJFILES += ledTask.o radioTask.o flightTask.o 
#OBJFILES += rc_pwm.o i2c.o bma180.o itg3200.o hmc5843.o
#OBJFILES += fixed_DCM.o fix16.o fix16_sqrt.o fix16_trig.o
#OBJFILES += pid.o
#SRCS = $(shell find CoOS | grep "\.c$$" | xargs basename)
#OBJFILES+= $(SRCS:.c=.o)

OBJS = $(addprefix ${OBJFOLDER}/, $(OBJFILES))

#INIT_OBJS= ${OBJFOLDER}/LPC11xx_handlers.o ${OBJFOLDER}/LPC1xxx_startup.o


##########################################################################
#LPC21ISP PROGRAMMING
##########################################################################
LPC21ISP = /usr/local/bin/lpc21isp
LPC21ISP_PORT = /dev/tty.usbserial-A101MWJ6
LPC21ISP_BAUD = 115200 #38400
LPC21ISP_XTAL = 12000
LPC21ISP_FLASHFILE = $(OBJFOLDER)/$(OUTFILE).bin
LPC21ISP_CONTROL = -control -RstDone
LPC21ISP_OPTIONS = -verify -term -bin




# Make all
##########################################################################
all: ${OBJFOLDER}           \
     ${OBJFOLDER}/$(OUTFILE).elf 

# Clean the stuff 
##########################################################################
clean:
	@rm -rf ${OBJFOLDER} ${wildcard *.bin} $(OUTFILE).elf


# Make the folder used to hold the objs
##########################################################################
${OBJFOLDER}:
	@mkdir ${OBJFOLDER}

# Make the efl and hex from the objects
##########################################################################
${OBJFOLDER}/$(OUTFILE).elf: ${INIT_OBJS} ${OBJS}


# Flash the board
##########################################################################
flash:
	@echo "  PROG   $(OUTFILE).bin"
	@ps axc | grep lpc21isp >/dev/null; if [ $$? -eq 0 ] ; then killall lpc21isp ; fi
	@echo '$(LPC21ISP) $(LPC21ISP_OPTIONS) $(LPC21ISP_CONTROL) $(LPC21ISP_DEBUG) "$(CURDIR)/$(LPC21ISP_FLASHFILE)" $(LPC21ISP_PORT) $(LPC21ISP_BAUD) $(LPC21ISP_XTAL)' > $(OBJFOLDER)/debugcommand.txt
	@chmod +x $(OBJFOLDER)/debugcommand.txt
	@open -a terminal.app $(OBJFOLDER)/debugcommand.txt


# Build the USB Binary
usb_prog: all
	@.bin/lpc-flash $(OBJFOLDER)/$(OUTFILE).bin $(OBJFOLDER)/$(OUTFILE).crc.bin
	@.bin/lpc-flash $(OBJFOLDER)/$(OUTFILE).bin /Volumes/CRP\ DISABLD/firmware.bin

# Build the files and program the board
##########################################################################
program:all flash



objdump: $(OBJFOLDER)/$(OUTFILE).dump



# Open a terminal using lpc21isp  This controls the bootloader so the
# board is not stuck in reset
##########################################################################
term: ${OBJFOLDER}
	@echo "TERM lpc21isp"
	@ps axc | grep lpc21isp >/dev/null; if [ $$? -eq 0 ] ; then killall lpc21isp ; fi		
	@echo '$(LPC21ISP) -termonly $(LPC21ISP_CONTROL) $(LPC21ISP_DEBUG) "$(CURDIR)/$(LPC21ISP_FLASHFILE)" $(LPC21ISP_PORT) $(LPC21ISP_BAUD) $(LPC21ISP_XTAL)' > $(OBJFOLDER)/debugcommand.txt
	@chmod +x $(OBJFOLDER)/debugcommand.txt
	@open -a terminal.app $(OBJFOLDER)/debugcommand.txt

size:
	@if [ -d objs ];\
	 then\
	  .bin/size_all; \
	 fi

# Print a variable
##########################################################################
print:
	@echo "INCLUDE DIRS $(INCLUDEDIRS)\n"
	@echo "OBJS $(OBJS)\n"
	@echo "VPATH $(VPATH)\n"


-include ${wildcard ${OBJFOLDER}/*.d} __dummy__





