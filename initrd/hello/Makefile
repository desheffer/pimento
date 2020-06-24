.DEFAULT_GOAL := all

PREFIX  ?=
CC      := $(PREFIX)gcc
LD      := $(PREFIX)ld
OBJCOPY := $(PREFIX)objcopy
OBJDUMP := $(PREFIX)objdump

ARCH := aarch64

IMG := hello

BUILD := build
SRC   := src

OBJECTS := $(patsubst $(SRC)/%.S, $(BUILD)/%.S.o, $(shell find $(SRC) -name '*.S'))
OBJECTS += $(patsubst $(SRC)/%.c, $(BUILD)/%.c.o, $(shell find $(SRC) -name '*.c'))

BASEFLAGS := -O2 -nostdlib -nostartfiles
BASEFLAGS += -ffreestanding -mgeneral-regs-only

WARNFLAGS := -Wall -Wextra
ASFLAGS   := $(BASEFLAGS) $(INCFLAGS)
CFLAGS    := $(BASEFLAGS) $(WARNFLAGS) $(INCFLAGS)
LDFLAGS   := $(BASEFLAGS) -lgcc

.PHONY: all
all: $(BUILD)/$(IMG).img

.PHONY: clean
clean:
	rm -rf $(BUILD)

$(BUILD)/$(IMG).img: $(BUILD)/$(IMG).elf
	$(PREFIX)objcopy $< -O binary $@

$(BUILD)/$(IMG).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BUILD)/%.S.o: $(SRC)/%.S $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%.c.o: $(SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
