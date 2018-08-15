TOOLCHAIN ?= aarch64-none-elf
CC        := $(TOOLCHAIN)-gcc
AR        := $(TOOLCHAIN)-ar

LIB := libc

BUILD   := build
INCLUDE := include
SRC     := .

INCLUDES := $(shell find $(INCLUDE) -name '*.h')

OBJECTS := $(patsubst $(SRC)/%.S, $(BUILD)/%.o, $(shell find $(SRC) -name '*.S'))
OBJECTS += $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(shell find $(SRC) -name '*.c'))
OBJECTS += $(patsubst $(SRC)/%.cc, $(BUILD)/%.o, $(shell find $(SRC) -name '*.cc'))

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
INCFLAGS  := -I$(INCLUDE)
ASFLAGS   := $(INCFLAGS) -D__ASSEMBLY__
CFLAGS    := $(BASEFLAGS) $(WARNFLAGS) $(INCFLAGS)
LDFLAGS   := $(BASEFLAGS)

.PHONY: all clean

all: $(BUILD)/$(LIB).a

clean:
	rm -rf $(BUILD)

$(BUILD)/$(LIB).a: $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)

$(BUILD)/%.o: $(SRC)/%.S $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
