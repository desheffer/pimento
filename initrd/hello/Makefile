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

BASEFLAGS := -O2

WARNFLAGS := -Wall -Wextra
ASFLAGS   := $(BASEFLAGS) $(INCFLAGS)
CFLAGS    := $(BASEFLAGS) $(WARNFLAGS) $(INCFLAGS)
LDFLAGS   := $(BASEFLAGS) -static

.PHONY: all
all: $(BUILD)/$(IMG).elf $(BUILD)/$(IMG).list

.PHONY: clean
clean:
	rm -rf $(BUILD)

$(BUILD)/$(IMG).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BUILD)/$(IMG).list: $(BUILD)/$(IMG).elf
	$(OBJDUMP) -D $< > $(BUILD)/$(IMG).list

$(BUILD)/%.S.o: $(SRC)/%.S $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%.c.o: $(SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
