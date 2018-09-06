TOOLCHAIN ?= aarch64-none-elf
CC        := $(TOOLCHAIN)-gcc
OBJCOPY   := $(TOOLCHAIN)-objcopy

IMG := kernel8

BUILD        := build
INCLUDE      := ../kernel/include
LIBC         := ../libc/build
SRC          := ../kernel
TEST_SRC     := .
TEST_INCLUDE := include

INCLUDES := $(shell find $(INCLUDE) -name '*.h')
INCLUDES += $(shell find $(TEST_INCLUDE) -name '*.h')

OBJECTS := $(patsubst $(SRC)/%.S, $(BUILD)/%_S.o, $(shell find $(SRC) -name '*.S'))
OBJECTS += $(patsubst $(SRC)/%.c, $(BUILD)/%_c.o, $(shell find $(SRC) -name '*.c'))
OBJECTS += $(patsubst $(TEST_SRC)/%.c, $(BUILD)/test/%_c.o, $(shell find $(TEST_SRC) -name '*.c'))
OBJECTS += $(LIBC)/libc.a

OBJECTS := $(filter-out $(BUILD)/kernel/main_c.o, $(OBJECTS))

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
INCFLAGS  := -I$(TEST_INCLUDE) -I$(INCLUDE) -I$(LIBC)/../include
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

$(BUILD)/%_S.o: $(SRC)/%.S
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%_c.o: $(SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/test/%_c.o: $(TEST_SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
