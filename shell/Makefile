TOOLCHAIN ?= aarch64-none-elf
CC        := $(TOOLCHAIN)-gcc
OBJCOPY   := $(TOOLCHAIN)-objcopy
OBJDUMP   := $(TOOLCHAIN)-objdump

IMG := shell

BUILD := build
LIBC  := ../libc/build
SRC   := .

OBJECTS += $(patsubst $(SRC)/%.c, $(BUILD)/%_c.o, $(shell find $(SRC) -name '*.c'))
OBJECTS += $(LIBC)/libc.a

BASEFLAGS := -O2 -nostdlib -nostartfiles
BASEFLAGS += -ffreestanding -fomit-frame-pointer
WARNFLAGS := -Wall -Wextra -Wformat=2 -Wcast-align -Wpointer-arith
WARNFLAGS += -Wredundant-decls -Wshadow -Wwrite-strings
WARNFLAGS += -Werror=implicit-function-declaration
INCFLAGS  := -I$(LIBC)/../include
CFLAGS    := $(BASEFLAGS) $(WARNFLAGS) $(INCFLAGS)
LDFLAGS   := $(BASEFLAGS)

.PHONY: all clean

all: $(BUILD)/$(IMG).img

clean:
	rm -rf $(BUILD)

$(BUILD)/$(IMG).img: $(BUILD)/$(IMG).elf
	$(OBJCOPY) $(BUILD)/$(IMG).elf -O binary $@
	$(OBJDUMP) -D $(BUILD)/$(IMG).elf > $(BUILD)/$(IMG).list

$(BUILD)/$(IMG).elf: $(OBJECTS) link-aarch64-elf.ld
	$(CC) $(LDFLAGS) $(OBJECTS) -Tlink-aarch64-elf.ld -lgcc -o $@

$(BUILD)/%_c.o: $(SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
