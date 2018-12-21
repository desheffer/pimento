TOOLCHAIN ?= aarch64-linux
CC        := $(TOOLCHAIN)-gcc
OBJCOPY   := $(TOOLCHAIN)-objcopy
OBJDUMP   := $(TOOLCHAIN)-objdump

BIN := shell

BUILD := build
SRC   := .

OBJECTS += $(patsubst $(SRC)/%.c, $(BUILD)/%.c.o, $(shell find $(SRC) -name '*.c'))

BASEFLAGS := -O2
WARNFLAGS := -Wall -Wextra -Wformat=2 -Wcast-align -Wpointer-arith
WARNFLAGS += -Wredundant-decls -Wshadow -Wwrite-strings
WARNFLAGS += -Werror=implicit-function-declaration
CFLAGS    := $(BASEFLAGS) $(WARNFLAGS)
LDFLAGS   := $(BASEFLAGS) -lgcc -static

.PHONY: all clean

all: $(BUILD)/$(BIN).elf $(BUILD)/$(BIN).list

clean:
	rm -rf $(BUILD)

$(BUILD)/$(BIN).list: $(BUILD)/$(BIN).elf
	$(OBJDUMP) -D $(BUILD)/$(BIN).elf > $(BUILD)/$(BIN).list

$(BUILD)/$(BIN).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BUILD)/%.c.o: $(SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
