# arm-bcm2708-linux-gnueabi.
ARMGNU ?= /home/robert/Desktop/rasp/gcc-arm-none-eabi-4_7-2013q3/bin/arm-none-eabi

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

# The names of all object files that must be generated.
# Primero el codigo en asm y después el C. De otra manera da error.
OBJECTS := $(patsubst $(SOURCE)%.S,$(BUILD)%.o,$(wildcard $(SOURCE)*/*.S)) $(patsubst $(SOURCE)%.c,$(BUILD)%.o,$(wildcard $(SOURCE)*/*.c))

# Rule to make all.
all: $(TARGET) $(LIST)

# Rule to remake everything. Does not include clean.
rebuild: all

# Rules

build/%.o: source/%.c
	$(ARMGNU)-gcc-4.7.4 -c -O0 -DRPIBPLUS -fno-builtin -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles -o $@ $<


# Rule to make the listing file.
$(LIST) : $(BUILD)output.elf
	$(ARMGNU)-objdump -g -d $(BUILD)output.elf > $(LIST)

# Rule to make the image file.
$(TARGET) : $(BUILD)output.elf
	$(ARMGNU)-objcopy $(BUILD)output.elf -O binary $(TARGET) 

# Rule to make the elf file (linkado).
$(BUILD)output.elf : $(OBJECTS) $(LINKER)
	$(ARMGNU)-ld $(OBJECTS) -Map $(MAP) -o $(BUILD)output.elf -T $(LINKER)

# Rule to make the object files (ensamblador).
$(BUILD)%.o: $(SOURCE)%.s $(BUILD)
	$(ARMGNU)-as -g -I $(SOURCE) $< -o $@

$(BUILD):
	mkdir $@z

# Rule to clean files.
clean : 
	-rm -rf $(BUILD)
	-rm -f $(TARGET)
	-rm -f $(LIST)
	-rm -f $(MAP)
	-mkdir build
	-mkdir build/apolo
	-mkdir build/hades
	-mkdir build/hefesto
	-mkdir build/poseidon
	-mkdir build/atenea
	-mkdir build/zeus
