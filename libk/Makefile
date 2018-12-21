TOOLCHAIN ?= aarch64-linux
CC        := $(TOOLCHAIN)-gcc
AR        := $(TOOLCHAIN)-ar

LIB := libk

BUILD   := build
INCLUDE := include
SRC     := .

INCLUDES := $(shell find $(INCLUDE) -name '*.h')

OBJECTS := $(patsubst $(SRC)/%.S, $(BUILD)/%.S.o, $(shell find $(SRC) -name '*.S'))
OBJECTS += $(patsubst $(SRC)/%.c, $(BUILD)/%.c.o, $(shell find $(SRC) -name '*.c'))
OBJECTS += $(patsubst $(SRC)/%.cc, $(BUILD)/%_cc.o, $(shell find $(SRC) -name '*.cc'))

BASEFLAGS := -O2 -nostdlib -nostartfiles
BASEFLAGS += -ffreestanding -fomit-frame-pointer
WARNFLAGS := -Wall -Wextra -Wformat=2 -Wcast-align -Wpointer-arith
WARNFLAGS += -Wredundant-decls -Wshadow -Wwrite-strings
WARNFLAGS += -Werror=implicit-function-declaration
INCFLAGS  := -I$(INCLUDE)
ASFLAGS   := $(INCFLAGS) -D__ASSEMBLY__
CFLAGS    := $(BASEFLAGS) $(WARNFLAGS) $(INCFLAGS)
LDFLAGS   := $(BASEFLAGS) -lgcc

.PHONY: all clean

all: $(BUILD)/$(LIB).a

clean:
	rm -rf $(BUILD)

$(BUILD)/$(LIB).a: $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)

$(BUILD)/%.S.o: $(SRC)/%.S $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%.c.o: $(SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
