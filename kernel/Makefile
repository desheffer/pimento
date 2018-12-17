TOOLCHAIN ?= aarch64-linux
CC        := $(TOOLCHAIN)-gcc
OBJCOPY   := $(TOOLCHAIN)-objcopy
OBJDUMP   := $(TOOLCHAIN)-objdump

IMG := kernel8

BUILD   := build
INCLUDE := include
LIBC    := ../libc/build
SRC     := .

INCLUDES := $(shell find $(INCLUDE) -name '*.h')

OBJECTS := $(patsubst $(SRC)/%.S, $(BUILD)/%_S.o, $(shell find $(SRC) -name '*.S'))
OBJECTS += $(patsubst $(SRC)/%.c, $(BUILD)/%_c.o, $(shell find $(SRC) -name '*.c'))
OBJECTS += $(LIBC)/libc.a
OBJECTS += $(BUILD)/shell.bin

BASEFLAGS := -O2 -nostdlib -nostartfiles
BASEFLAGS += -ffreestanding -fomit-frame-pointer
WARNFLAGS := -Wall -Wextra -Wformat=2 -Wcast-align -Wpointer-arith
WARNFLAGS += -Wredundant-decls -Wshadow -Wwrite-strings
WARNFLAGS += -Werror=implicit-function-declaration
INCFLAGS  := -I$(INCLUDE) -I$(LIBC)/../include
ASFLAGS   := $(INCFLAGS) -D__ASSEMBLY__
CFLAGS    := $(BASEFLAGS) $(WARNFLAGS) $(INCFLAGS)
LDFLAGS   := $(BASEFLAGS) -lgcc

.PHONY: all clean

all: $(BUILD)/$(IMG).img

clean:
	rm -rf $(BUILD)

$(BUILD)/$(IMG).img: $(BUILD)/$(IMG).elf
	$(OBJCOPY) $(BUILD)/$(IMG).elf -O binary $@
	$(OBJDUMP) -D $(BUILD)/$(IMG).elf > $(BUILD)/$(IMG).list

$(BUILD)/$(IMG).elf: $(OBJECTS) link-aarch64-elf.ld
	$(CC) $(LDFLAGS) $(OBJECTS) -Tlink-aarch64-elf.ld -o $@

$(BUILD)/%_S.o: $(SRC)/%.S $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%_c.o: $(SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.bin: ../*/build/%.img
	$(OBJCOPY) --input-target binary --output-target elf64-littleaarch64 --binary-architecture aarch64 $< $@
