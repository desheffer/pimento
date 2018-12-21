TOOLCHAIN ?= aarch64-linux
CC        := $(TOOLCHAIN)-gcc
OBJCOPY   := $(TOOLCHAIN)-objcopy
OBJDUMP   := $(TOOLCHAIN)-objdump

IMG := kernel8

BUILD        := build
INCLUDE      := ../kernel/include
LIBK         := ../libk/build
SRC          := ../kernel
TEST_SRC     := .
TEST_INCLUDE := include

INCLUDES := $(shell find $(INCLUDE) -name '*.h')
INCLUDES += $(shell find $(TEST_INCLUDE) -name '*.h')

OBJECTS := $(patsubst $(SRC)/%.S, $(BUILD)/%_S.o, $(shell find $(SRC) -name '*.S'))
OBJECTS += $(patsubst $(SRC)/%.c, $(BUILD)/%_c.o, $(shell find $(SRC) -name '*.c'))
OBJECTS += $(patsubst $(TEST_SRC)/%.c, $(BUILD)/test/%_c.o, $(shell find $(TEST_SRC) -name '*.c'))
OBJECTS += $(LIBK)/libk.a

OBJECTS := $(filter-out $(BUILD)/kernel/main_c.o, $(OBJECTS))

BASEFLAGS := -O2 -nostdlib -nostartfiles
BASEFLAGS += -ffreestanding -fomit-frame-pointer
WARNFLAGS := -Wall -Wextra -Wformat=2 -Wcast-align -Wpointer-arith
WARNFLAGS += -Wredundant-decls -Wshadow -Wwrite-strings
WARNFLAGS += -Werror=implicit-function-declaration
INCFLAGS  := -I$(TEST_INCLUDE) -I$(INCLUDE) -I$(LIBK)/../include
ASFLAGS   := $(INCFLAGS) -D__ASSEMBLY__
CFLAGS    := $(BASEFLAGS) $(WARNFLAGS) $(INCFLAGS)
LDFLAGS   := $(BASEFLAGS) -lgcc

.PHONY: all clean

all: $(BUILD)/$(IMG).img

clean:
	rm -rf $(BUILD)

$(BUILD)/$(IMG).img: $(BUILD)/$(IMG).elf
	$(OBJCOPY) $(BUILD)/$(IMG).elf -O binary $@
	$(OBJDUMP) -D $(BUILD)/$(IMG).elf > $(BUILD)/$(IMG).list

$(BUILD)/$(IMG).elf: $(OBJECTS) ../kernel/link-aarch64-elf.ld
	$(CC) $(LDFLAGS) $(OBJECTS) -T../kernel/link-aarch64-elf.ld -o $@

$(BUILD)/%_S.o: $(SRC)/%.S
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%_c.o: $(SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/test/%_c.o: $(TEST_SRC)/%.c $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
