TOOLCHAIN ?= aarch64-none-elf
CC        := $(TOOLCHAIN)-gcc
OBJCOPY   := $(TOOLCHAIN)-objcopy
OBJDUMP   := $(TOOLCHAIN)-objdump

IMG := shell

BUILD   := build
INCLUDE := include
LIBC    := ../libc/build
SRC     := .

INCLUDES := $(shell find $(INCLUDE) -name '*.h')

OBJECTS += $(patsubst $(SRC)/%.c, $(BUILD)/%_c.o, $(shell find $(SRC) -name '*.c'))
OBJECTS += $(LIBC)/libc.a

BASEFLAGS := -O2 -fpic -nostdlib
BASEFLAGS += -nostartfiles -ffreestanding -nodefaultlibs
BASEFLAGS += -fno-builtin -fomit-frame-pointer -mcpu=cortex-a53
WARNFLAGS := -Wall -Wextra -Wshadow -Wcast-align -Wwrite-strings
WARNFLAGS += -Wredundant-decls -Winline
WARNFLAGS += -Wno-attributes -Wno-deprecated-declarations
WARNFLAGS += -Wno-div-by-zero -Wno-endif-labels -Wfloat-equal
WARNFLAGS += -Wformat=2 -Wno-format-extra-args -Winit-self
WARNFLAGS += -Winvalid-pch -Wmissing-format-attribute
WARNFLAGS += -Wmissing-include-dirs -Wno-multichar
WARNFLAGS += -Wredundant-decls -Wshadow
WARNFLAGS += -Wno-sign-compare -Wswitch -Wsystem-headers
WARNFLAGS += -Wno-pragmas -Wno-unused-but-set-parameter
WARNFLAGS += -Wno-unused-but-set-variable -Wno-unused-result
WARNFLAGS += -Wwrite-strings -Wdisabled-optimization -Wpointer-arith
INCFLAGS  := -I$(INCLUDE) -I$(LIBC)/../include
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
