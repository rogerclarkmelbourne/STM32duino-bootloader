# Makefile skeleton adapted from Peter Harrison's - www.micromouse.com

# MCU name and submodel
MCU      = cortex-m3
SUBMDL   = stm32f103

# toolchain (using code sourcery now)
TCHAIN = arm-none-eabi
THUMB    = -mthumb
THUMB_IW = -mthumb-interwork

# Target file name (without extension).
BUILDDIR = build
TARGET = $(BUILDDIR)/maple_boot

ST_LIB = stm32_lib
ST_USB = usb_lib

# Optimization level [0,1,2,3,s]
OPT ?= s
DEBUG =
#DEBUG = dwarf-2

INCDIRS = ./$(ST_LIB) ./$(ST_USB)

CFLAGS = $(DEBUG)
CFLAGS += -O$(OPT)
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wall -Wimplicit
CFLAGS += -Wcast-align
CFLAGS += -Wpointer-arith -Wswitch
CFLAGS += -Wredundant-decls -Wreturn-type -Wshadow -Wunused
CFLAGS += -Wa,-adhlns=$(BUILDDIR)/$(subst $(suffix $<),.lst,$<)
CFLAGS += $(patsubst %,-I%,$(INCDIRS))

# Aeembler Flags
ASFLAGS = -Wa,-adhlns=$(BUILDDIR)/$(<:.s=.lst)#,--g$(DEBUG)

LDFLAGS = -nostartfiles -Wl,-Map=$(TARGET).map,--cref,--gc-sections
LDFLAGS += -lc -lgcc



# Define programs and commands.
SHELL = sh
CC = $(TCHAIN)-gcc
CPP = $(TCHAIN)-g++
AR = $(TCHAIN)-ar
OBJCOPY = $(TCHAIN)-objcopy
OBJDUMP = $(TCHAIN)-objdump
SIZE = $(TCHAIN)-size
NM = $(TCHAIN)-nm
REMOVE = rm -f
REMOVEDIR = rm -r
COPY = cp

# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = "-------- begin --------"
MSG_ETAGS = Created TAGS File
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before:
MSG_SIZE_AFTER = Size after:
MSG_FLASH = Creating load file for Flash:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:

# Combine all necessary flags and optional flags.
# Add target processor to flags.
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d
ALL_CFLAGS  = -g -mcpu=$(MCU) $(THUMB_IW) -I. $(CFLAGS) $(TARGETFLAGS) $(GENDEPFLAGS)
ALL_ASFLAGS = -g -mcpu=$(MCU) $(THUMB_IW) -I. -x assembler-with-cpp $(ASFLAGS)

# --------------------------------------------- #
# file management
ASRC = $(ST_LIB)/c_only_startup.s $(ST_LIB)/cortexm3_macro.s

STM32SRCS =

_STM32USBSRCS = usb_regs.c \
usb_int.c \
usb_init.c \
usb_core.c \
usb_mem.c

STM32USBSRCS = $(patsubst %, $(ST_USB)/%,$(_STM32USBSRCS))

SRCS = usb.c usb_callbacks.c usb_descriptor.c main.c hardware.c dfu.c


SRC = $(SRCS) $(STM32SRCS) $(STM32USBSRCS)

# Define all object files.
_COBJ =  $(SRC:.c=.o)
_AOBJ =  $(ASRC:.s=.o)
COBJ = $(patsubst %, $(BUILDDIR)/%,$(_COBJ))
AOBJ = $(patsubst %, $(BUILDDIR)/%,$(_AOBJ))

# Define all listing files.
_LST  =  $(ASRC:.s=.lst)
_LST +=  $(SRC:.c=.lst)
LST = $(patsubst %, $(BUILDDIR)/%,$(_LST))

# Display size of file.
HEXSIZE = $(SIZE) --target=binary $(TARGET).hex
ELFSIZE = $(SIZE) -A $(TARGET).elf

# go!
all: begin gccversion build sizeafter finished end

maple-mini: begin clean gccversion build_maple-mini sizeafter finished  copy_maple_mini end
maple-rev3: begin clean gccversion build_maple-rev3 sizeafter finished  copy_maple-rev3 end
maple-rev5: begin clean gccversion build_maple-rev5 sizeafter finished  copy_maple-rev5 end
generic-pc13: begin clean gccversion build_generic-pc13 sizeafter finished  copy_generic-pc13 end
generic-pg15: begin clean gccversion build_generic-pg15 sizeafter finished  copy_generic-pg15 end
generic-pd2: begin clean gccversion build_generic-pd2 sizeafter finished  copy_generic-pd2 end
generic-pd1: begin clean gccversion build_generic-pd1 sizeafter finished  copy_generic-pd1 end
generic-pa1: begin clean gccversion build_generic-pa1 sizeafter finished  copy_generic-pa1 end
generic-pa1-button-pa8: begin clean gccversion build_generic-pa1-button-pa8 sizeafter finished  copy_generic-pa1-button-pa8 end
generic-pb9: begin clean gccversion build_generic-pb9 sizeafter finished  copy_generic-pb9 end
generic-pe2: begin clean gccversion build_generic-pe2 sizeafter finished  copy_generic-pe2 end
generic-pa9: begin clean gccversion build_generic-pa9 sizeafter finished  copy_generic-pa9 end
generic-pe5: begin clean gccversion build_generic-pe5 sizeafter finished  copy_generic-pe5 end
generic-pe5-button-pa0: begin clean gccversion build_generic-pe5-button-pa0 sizeafter finished  copy_generic-pe5-button-pa0 end
generic-pb7: begin clean gccversion build_generic-pb7 sizeafter finished  copy_generic-pb7 end
generic-pb0: begin clean gccversion build_generic-pb0 sizeafter finished  copy_generic-pb0 end
stbee :  begin clean gccversion build_stbee sizeafter finished  copy_stbee end
naze32: begin clean gccversion build_naze32 sizeafter finished  copy_naze32 end
generic-pb12: begin clean gccversion build_generic-pb12 sizeafter finished  copy_generic-pb12 end
hytiny-stm32f103t: begin clean gccversion build_hytiny-stm32f103t sizeafter finished  copy_hytiny-stm32f103t end
dso138: begin clean gccversion build_dso138 sizeafter finished  copy_dso138 end
gd32f1-generic-pc13: begin clean gccversion build_gd32f1-generic-pc13 sizeafter finished  copy_gd32f1-generic-pc13 end
gd32f1-frankenmaple: begin clean gccversion build_gd32f1-frankenmaple sizeafter finished  copy_gd32f1-frankenmaple end

build: elf bin lss sym

build_maple-mini: TARGETFLAGS= -DTARGET_MAPLE_MINI $(DEFINES)
# Set the linker script
build_maple-mini: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld

build_maple-mini: elf bin lss sym
copy_maple_mini:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/maple_mini_boot20.bin
	@echo

build_maple-rev3: TARGETFLAGS= -DTARGET_MAPLE_REV3 $(DEFINES)
# Set the linker script
build_maple-rev3: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld

build_maple-rev3: elf bin lss sym
copy_maple-rev3:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/maple_rev3_boot20.bin
	@echo

build_maple-rev5: TARGETFLAGS= -DTARGET_MAPLE_REV5 $(DEFINES)
# Set the linker script
build_maple-rev5: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_maple-rev5: elf bin lss sym
copy_maple-rev5:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/maple_rev5_boot20.bin
	@echo


build_generic-pc13: TARGETFLAGS= -DTARGET_GENERIC_F103_PC13 $(DEFINES)
# Set the linker script
build_generic-pc13: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pc13: elf bin lss sym
copy_generic-pc13:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pc13.bin
	@echo

build_generic-pg15: TARGETFLAGS= -DTARGET_GENERIC_F103_PG15 $(DEFINES)
# Set the linker script
build_generic-pg15: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pg15: elf bin lss sym
copy_generic-pg15:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pg15.bin
	@echo


build_generic-pd2: TARGETFLAGS= -DTARGET_GENERIC_F103_PD2 $(DEFINES)
# Set the linker script
build_generic-pd2: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pd2: elf bin lss sym
copy_generic-pd2:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pd2.bin
	@echo


build_generic-pd1: TARGETFLAGS= -DTARGET_GENERIC_F103_PD1 $(DEFINES)
# Set the linker script
build_generic-pd1: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pd1: elf bin lss sym
copy_generic-pd1:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pd1.bin
	@echo

build_generic-pa1: TARGETFLAGS= -DTARGET_GENERIC_F103_PA1 $(DEFINES)
# Set the linker script
build_generic-pa1: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pa1: elf bin lss sym
copy_generic-pa1:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pa1.bin
	@echo

build_generic-pa1-button-pa8: TARGETFLAGS= -DTARGET_GENERIC_F103_PA1_BUTTON_PA8 $(DEFINES)
# Set the linker script
build_generic-pa1-button-pa8: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pa1-button-pa8: elf bin lss sym
copy_generic-pa1-button-pa8:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pa1_button_pa8.bin
	@echo

build_generic-pb9: TARGETFLAGS= -DTARGET_GENERIC_F103_PB9 $(DEFINES)
# Set the linker script
build_generic-pb9: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pb9: elf bin lss sym
copy_generic-pb9:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pb9.bin
	@echo


build_generic-pe2: TARGETFLAGS= -DTARGET_GENERIC_F103_PE2 $(DEFINES)
# Set the linker script
build_generic-pe2: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pe2: elf bin lss sym
copy_generic-pe2:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pe2.bin
	@echo


build_generic-pa9: TARGETFLAGS= -DTARGET_GENERIC_F103_PA9 $(DEFINES)
# Set the linker script
build_generic-pa9: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pa9: elf bin lss sym
copy_generic-pa9:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pa9.bin
	@echo


build_generic-pe5: TARGETFLAGS= -DTARGET_GENERIC_F103_PE5 $(DEFINES)
# Set the linker script
build_generic-pe5: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pe5: elf bin lss sym
copy_generic-pe5:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pe5.bin
	@echo


build_generic-pe5-button-pa0: TARGETFLAGS= -DTARGET_GENERIC_F103_PE5_BUTTON_PA0 $(DEFINES)
# Set the linker script
build_generic-pe5-button-pa0: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pe5-button-pa0: elf bin lss sym
copy_generic-pe5-button-pa0:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pe5_button_pa0.bin
	@echo


build_generic-pb7: TARGETFLAGS= -DTARGET_GENERIC_F103_PB7 $(DEFINES)
# Set the linker script
build_generic-pb7: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pb7: elf bin lss sym
copy_generic-pb7:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pb7.bin
	@echo

build_generic-pb0: TARGETFLAGS= -DTARGET_GENERIC_F103_PB0 $(DEFINES)
# Set the linker script
build_generic-pb0: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pb0: elf bin lss sym
copy_generic-pb0:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pb0.bin
	@echo


build_stbee: TARGETFLAGS= -DTARGET_STBEE $(DEFINES)
# Set the linker script
build_stbee: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_stbee: elf bin lss sym
copy_stbee:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/stbee_boot20.bin
	@echo

build_naze32: TARGETFLAGS= -DTARGET_NAZE32 $(DEFINES)
# Set the linker script
build_naze32: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_naze32: elf bin lss sym
copy_naze32:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/naze32_boot20.bin
	@echo

build_generic-pb12: TARGETFLAGS= -DTARGET_GENERIC_F103_PB12  $(DEFINES)
# Set the linker script
build_generic-pb12: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_generic-pb12: elf bin lss sym
copy_generic-pb12:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_pb12.bin
	@echo

build_hytiny-stm32f103t: TARGETFLAGS= -DTARGET_HYTINY_STM32F103T $(DEFINES)
# Set the linker script
build_hytiny-stm32f103t: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_hytiny-stm32f103t: elf bin lss sym
copy_hytiny-stm32f103t:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/generic_boot20_hytiny.bin
	@echo

build_dso138: TARGETFLAGS= -DTARGET_DSO138 $(DEFINES)
# Set the linker script
build_dso138: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_dso138: elf bin lss sym
copy_dso138:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/dso138_boot20.bin
	@echo

build_gd32f1-generic-pc13: TARGETFLAGS= -DTARGET_GD32F1_GENERIC_F103_PC13 $(DEFINES)
# Set the linker script
build_gd32f1-generic-pc13: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_gd32f1-generic-pc13: elf bin lss sym
copy_gd32f1-generic-pc13:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/gd32f1_generic_boot20_pc13.bin
	@echo

build_gd32f1-frankenmaple: TARGETFLAGS= -DTARGET_GD32F1_FRANKENMAPLE $(DEFINES)
# Set the linker script
build_gd32f1-frankenmaple: LDFLAGS +=-T$(ST_LIB)/c_only_md_high_density.ld
build_gd32f1-frankenmaple: elf bin lss sym
copy_gd32f1-frankenmaple:
	@echo
	@echo "Copying to binaries folder"
	@echo
	cp $(TARGET).bin bootloader_only_binaries/gd32f1_frankenmaple.bin
	@echo


bin: $(TARGET).bin
elf: $(TARGET).elf
lss: $(TARGET).lss
sym: $(TARGET).sym
dfu: $(TARGET).bin
	sudo dfu-util -d 0110:1001 -a 0 -D $(TARGET).bin

begin:
	mkdir -p build/stm32_lib
	mkdir -p build/usb_lib
	@echo --
	@echo $(MSG_BEGIN)
	@echo $(COBJ)

finished:
	@echo $(MSG_ERRORS_NONE)
tags:
	etags `find . -name "*.c" -o -name "*.cpp" -o -name "*.h"`
	@echo $(MSG_ETAGS)
end:
	@echo $(MSG_END)
	@echo
sizeafter:
	@if [ -f $(TARGET).elf ]; then echo; echo $(MSG_SIZE_AFTER); $(ELFSIZE); echo; fi
gccversion:
	@$(CC) --version

program:
	@echo "Flash-programming with OpenOCD"
	cp $(TARGET).bin flash/tmpflash.bin
	cd flash && openocd -f flash.cfg

program_serial:
	@echo "Flash-programming with stm32loader.py"
	./flash/stm32loader.py -p /dev/ttyUSB0 -evw build/maple_boot.bin

debug: $(TARGET).bin
	@echo "Flash-programming with OpenOCD - DEBUG"
	cp $(TARGET).bin flash/tmpflash.bin
	cd flash && openocd -f debug.cfg

install: $(TARGET).bin
	cp $(TARGET).bin build/main.bin
	openocd -f flash/perry_flash.cfg

run: $(TARGET).bin
	openocd -f flash/run.cfg

# Create final output file (.hex) from ELF output file.
%.hex: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O binary $< $@

# Create final output file (.bin) from ELF output file.
%.bin: %.elf
	@echo
	@echo $(MSG_FLASH) $@
	$(OBJCOPY) -O binary $< $@


# Create extended listing file from ELF output file.
# testing: option -C
%.lss: %.elf
	@echo
	@echo $(MSG_EXTENDED_LISTING) $@
	$(OBJDUMP) -h -S -D $< > $@


# Create a symbol table from ELF output file.
%.sym: %.elf
	@echo
	@echo $(MSG_SYMBOL_TABLE) $@
	$(NM) -n $< > $@


# Link: create ELF output file from object files.
.SECONDARY : $(TARGET).elf
.PRECIOUS : $(COBJ) $(AOBJ)

%.elf:  $(COBJ) $(AOBJ)
	@echo
	@echo $(MSG_LINKING) $@
	$(CC) $(THUMB) $(ALL_CFLAGS) $(AOBJ) $(COBJ) --output $@ $(LDFLAGS)

# Compile: create object files from C source files. ARM/Thumb
$(COBJ) : $(BUILDDIR)/%.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) -c $(THUMB) $(ALL_CFLAGS) $< -o $@

# Assemble: create object files from assembler source files. ARM/Thumb
$(AOBJ) : $(BUILDDIR)/%.o : %.s
	@echo
	@echo $(MSG_ASSEMBLING) $<
	$(CC) -c $(THUMB) $(ALL_ASFLAGS) $< -o $@

clean: begin clean_list finished end

clean_list :
	@echo
	@echo $(MSG_CLEANING)
	$(REMOVE) $(TARGET).hex
	$(REMOVE) $(TARGET).bin
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).elf
	$(REMOVE) $(TARGET).map
	$(REMOVE) $(TARGET).obj
	$(REMOVE) $(TARGET).a90
	$(REMOVE) $(TARGET).sym
	$(REMOVE) $(TARGET).lnk
	$(REMOVE) $(TARGET).lss
	$(REMOVE) $(COBJ)
	$(REMOVE) $(AOBJ)
	$(REMOVE) $(LST)
	$(REMOVE) flash/tmpflash.bin
#   $(REMOVE) $(SRC:.c=.s)
#   $(REMOVE) $(SRC:.c=.d)
	$(REMOVE) .dep/*

# Include the dependency files.
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)


# Listing of phony targets.
.PHONY : all begin finish tags end sizeafter gccversion \
build elf hex bin lss sym clean clean_list program cscope

cscope:
	rm -rf *.cscope
	find . -iname "*.[hcs]" | grep -v examples | xargs cscope -R -b
