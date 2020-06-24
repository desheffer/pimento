.DEFAULT_GOAL := all

PREFIX  ?=
CC      := $(PREFIX)gcc
LD      := $(PREFIX)ld
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
OBJECTS += $(BUILD)/initrd.tar.o

BASEFLAGS := -O2 -nostdlib -nostartfiles
BASEFLAGS += -ffreestanding -mgeneral-regs-only
BASEFLAGS += -DRASPBERRY_PI_3

WARNFLAGS := -Wall -Wextra -Wcast-align -Wformat=2 -Wpointer-arith
WARNFLAGS += -Wredundant-decls -Wshadow -Wwrite-strings
INCFLAGS  := -I$(INCLUDE) -I$(INCLUDE)/arch/$(ARCH)
ASFLAGS   := $(BASEFLAGS) $(INCFLAGS)
CFLAGS    := $(BASEFLAGS) $(WARNFLAGS) $(INCFLAGS)
LDFLAGS   := $(BASEFLAGS) -Ttext=0xFFFF000000080000

.PHONY: all
all: $(BUILD)/$(IMG).img $(BUILD)/$(IMG).list

.PHONY: clean
clean:
	rm -rf $(BUILD)

$(BUILD)/$(IMG).img: $(BUILD)/$(IMG).elf
	$(OBJCOPY) $< -O binary $@

$(BUILD)/$(IMG).list: $(BUILD)/$(IMG).elf
	$(OBJDUMP) -D $< > $(BUILD)/$(IMG).list

$(BUILD)/$(IMG).elf: $(OBJECTS) $(SRC)/arch/$(ARCH)/link.ld
	$(CC) $(LDFLAGS) $(OBJECTS) -T $(SRC)/arch/$(ARCH)/link.ld -o $@

$(BUILD)/%.S.o: $(SRC)/%.S $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%.c.o: $(SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/initrd.tar.o: $(BUILD)/initrd.tar
	$(LD) -r -b binary -o $@ $<

$(BUILD)/initrd.tar: ../initrd/build/initrd.tar
	cp $< $@
