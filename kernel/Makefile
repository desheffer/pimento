.DEFAULT_GOAL := all

PREFIX  ?=
CC      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy
OBJDUMP := $(PREFIX)objdump

ARCH := aarch64

IMG := kernel8

BUILD   := build
INCLUDE := include
SRC     := src

INCLUDES := $(shell find $(INCLUDE) -name '*.h')

OBJECTS := $(patsubst $(SRC)/%.S, $(BUILD)/%.S.o, $(shell find $(SRC) -name '*.S'))
OBJECTS += $(patsubst $(SRC)/%.c, $(BUILD)/%.c.o, $(shell find $(SRC) -name '*.c'))

BASEFLAGS := -O2 -nostdlib -nostartfiles
BASEFLAGS += -ffreestanding -mgeneral-regs-only
BASEFLAGS += -DRASPBERRY_PI_3

WARNFLAGS := -Wall -Wextra -Wcast-align -Wformat=2 -Wpointer-arith
WARNFLAGS += -Wredundant-decls -Wshadow -Wwrite-strings
INCFLAGS  := -I$(INCLUDE) -I$(INCLUDE)/arch/$(ARCH)
ASFLAGS   := $(BASEFLAGS) $(INCFLAGS)
CFLAGS    := $(BASEFLAGS) $(WARNFLAGS) $(INCFLAGS)
LDFLAGS   := $(BASEFLAGS) -lgcc

.PHONY: all
all: $(BUILD)/$(IMG).img $(BUILD)/$(IMG).list

.PHONY: all
clean:
	rm -rf $(BUILD)

$(BUILD)/$(IMG).img: $(BUILD)/$(IMG).elf
	$(OBJCOPY) $(BUILD)/$(IMG).elf -O binary $@

$(BUILD)/$(IMG).list: $(BUILD)/$(IMG).elf
	$(OBJDUMP) -D $(BUILD)/$(IMG).elf > $(BUILD)/$(IMG).list

$(BUILD)/$(IMG).elf: $(OBJECTS) $(SRC)/arch/$(ARCH)/link.ld
	$(CC) $(LDFLAGS) $(OBJECTS) -T $(SRC)/arch/$(ARCH)/link.ld -o $@

$(BUILD)/%.S.o: $(SRC)/%.S $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%.c.o: $(SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.bin: $(BUILD)/%.elf
	$(OBJCOPY) --input-target binary --binary-architecture $(ARCH) $< $@
