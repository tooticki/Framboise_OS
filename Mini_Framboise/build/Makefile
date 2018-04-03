# How to compile C or assembly? (Maybe remove -fpic -ffreestanding if things don't work.)
CC=arm-none-eabi-gcc
CFLAGS=-O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -fpic -ffreestanding -std=gnu99

# How to link several .o together? (When in doubt -> same flags that work for a single-file compile)
LD=arm-none-eabi-gcc
LDFLAGS=-O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -fpic -ffreestanding -std=gnu99

# How to convert an ELF Linux executable to a bare bunch of machine code? (OC=ObjCopy)
OC=arm-none-eabi-objcopy
OCFLAGS=-O binary

# What is the list of files we need to compile?
# Replace '.' here with the top directory that contains all the sources
# (typically the root of the project, relative to the makefile).
C_SOURCES=$(shell find '../src' -name '*.c')
ASM_SOURCES=$(shell find '../src' -name '*.S')

# What will we compile them into?
C_OBJECTS=$(C_SOURCES:%.c=%.o)
ASM_OBJECTS=$(ASM_SOURCES:%.S=%.o)

# What's the name of the final result?
KERNEL=kernel

# Now the rules. In those rules,
# $@ == output
# $< == first input (usually there is only one, so it's fine)
# $^ == all the inputs, separated by spaces
# The general format is:
# OUTPUT: INPUTS
# 	COMMAND
#  /\
# (this is a tab haracter, NOT spaces.)

# The first rule is selected by default if we just type "make".
$(KERNEL).img: $(KERNEL).elf
	$(OC) $< $(OCFLAGS) $@

$(KERNEL).elf: $(C_OBJECTS) $(ASM_OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.S
	$(CC) $(CFLAGS) -c $< -o $@

terminal:
	sudo screen /dev/ttyUSB0 115200


# â€œcleanâ€ is not an actual output, but if we ask to generate â€œcleanâ€,
# the command will remove all compiled files (including the kernel).
clean:
	rm -f $(ASM_OBJECTS) $(C_OBJECTS) $(KERNEL)
