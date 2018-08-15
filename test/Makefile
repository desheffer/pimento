TOOLCHAIN ?= aarch64-none-elf
CC        := $(TOOLCHAIN)-gcc
OBJCOPY   := $(TOOLCHAIN)-objcopy

IMG := kernel8

BUILD   := build
INCLUDE := ../kernel/include
LIBC    := ../libc/build
SRC     := ../kernel
TEST    := .

INCLUDES := $(shell find $(INCLUDE) -name '*.h')
INCLUDES += $(shell find $(TEST) -name '*.h')

OBJECTS := $(patsubst $(SRC)/%.S, $(BUILD)/%.o, $(shell find $(SRC) -name '*.S'))
OBJECTS += $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(shell find $(SRC) -name '*.c'))
OBJECTS += $(patsubst $(TEST)/%.c, $(BUILD)/test/%.o, $(shell find $(TEST) -name '*.c'))
OBJECTS += $(LIBC)/libc.a

OBJECTS := $(filter-out $(BUILD)/kernel/main.o, $(OBJECTS))

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
INCFLAGS  := -I$(TEST) -I$(INCLUDE) -I$(LIBC)/../include
ASFLAGS   := $(INCFLAGS) -D__ASSEMBLY__
CFLAGS    := $(BASEFLAGS) $(WARNFLAGS) $(INCFLAGS)
LDFLAGS   := $(BASEFLAGS)

.PHONY: all clean

all: $(BUILD)/$(IMG).img

clean:
	rm -rf $(BUILD)

$(BUILD)/$(IMG).img: $(BUILD)/$(IMG).elf
	$(OBJCOPY) $(BUILD)/$(IMG).elf -O binary $@

$(BUILD)/$(IMG).elf: $(OBJECTS) ../kernel/link-aarch64-elf.ld
	$(CC) $(LDFLAGS) $(OBJECTS) -T../kernel/link-aarch64-elf.ld -lgcc -o $@

$(BUILD)/%.o: $(SRC)/%.S
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/test/%.o: $(TEST)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
