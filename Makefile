# arm-bcm2708-linux-gnueabi.
ARMGNU ?= arm-none-eabi

# The intermediate directory for compiled object files.
BUILD = build/

# The directory in which source files are stored.
SOURCE = source/

# The name of the output file to generate.
TARGET = kernel.img

# The name of the assembler listing file to generate.
LIST = kernel.list

# The name of the map file to generate.
MAP = kernel.map

# The name of the linker script to use.
LINKER = kernel.ld

# The names of all object files that must be generated. Deduced from the 
OBJECTS := build/boot.o build/rpi-gpuMailbox.o build/divisions.o build/main.o build/rpi-gpio.o build/rpi-systimer.o build/gpuFrameBuffer.o build/rpi-atags.o build/string.o build/screenbackground.o 

# Rule to make all.
all: $(TARGET) $(LIST)

# Rule to remake everything. Does not include clean.
rebuild: all

# Rules

build/main.o: source/program.c
	$(ARMGNU)-gcc -g -c -O2 -DRPIBPLUS -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -o build/main.o source/program.c

build/rpi-gpio.o: source/rpi-gpio.h source/rpi-gpio.c
	$(ARMGNU)-gcc -g -c -O2 -DRPIBPLUS -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -o build/rpi-gpio.o source/rpi-gpio.c

build/rpi-systimer.o: source/rpi-systimer.h source/rpi-systimer.c
	$(ARMGNU)-gcc -g -c -O2 -DRPIBPLUS -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -o build/rpi-systimer.o source/rpi-systimer.c

build/gpuFrameBuffer.o: source/gpuFrameBuffer.h source/gpuFrameBuffer.c
	$(ARMGNU)-gcc -g -c -O2 -DRPIBPLUS -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -o build/gpuFrameBuffer.o source/gpuFrameBuffer.c

build/rpi-gpuMailbox.o: source/rpi-gpuMailbox.S
	$(ARMGNU)-gcc -g -c -O2 -DRPIBPLUS -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -o build/rpi-gpuMailbox.o source/rpi-gpuMailbox.S

build/rpi-divisions.o: source/divisions.S
	$(ARMGNU)-gcc -g -c -O2 -DRPIBPLUS -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -o build/divisions.o source/divisions.S

build/rpi-atags.o: source/rpi-atags.h source/rpi-atags.c
	$(ARMGNU)-gcc -c -O2 -DRPIBPLUS -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -o build/rpi-atags.o source/rpi-atags.c

build/string.o: source/string.h source/string.c
	$(ARMGNU)-gcc -g -c -O2 -DRPIBPLUS -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -o build/string.o source/string.c

build/screenbackground.o: source/screenbackground.h source/screenbackground.c
	$(ARMGNU)-gcc -g -c -O2 -DRPIBPLUS -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -o build/screenbackground.o source/screenbackground.c	


# Rule to make the listing file.
$(LIST) : $(BUILD)output.elf
	$(ARMGNU)-objdump -g -d $(BUILD)output.elf > $(LIST)

# Rule to make the image file.
$(TARGET) : $(BUILD)output.elf
	$(ARMGNU)-objcopy $(BUILD)output.elf -O binary $(TARGET) 

# Rule to make the elf file.
$(BUILD)output.elf : $(OBJECTS) $(LINKER)
	$(ARMGNU)-ld $(OBJECTS) -Map $(MAP) -o $(BUILD)output.elf -T $(LINKER)

# Rule to make the object files.
$(BUILD)%.o: $(SOURCE)%.s $(BUILD)
	$(ARMGNU)-as -g -I $(SOURCE) $< -o $@

$(BUILD):
	mkdir $@

# Rule to clean files.
clean : 
	-rm -rf $(BUILD)
	-rm -f $(TARGET)
	-rm -f $(LIST)
	-rm -f $(MAP)
	-mkdir build
