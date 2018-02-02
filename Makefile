TOOLCHAIN ?= arm-none-eabi
CC        := $(TOOLCHAIN)-g++
OBJCOPY   := $(TOOLCHAIN)-objcopy

IMG := kernel7

BUILD   := build
INCLUDE := include
SRC     := src

INCLUDES := $(wildcard $(INCLUDE)/*.h)

OBJECTS := $(patsubst $(SRC)/%.S, $(BUILD)/%.o, $(wildcard $(SRC)/*.S))
OBJECTS += $(patsubst $(SRC)/%.cc, $(BUILD)/%.o, $(wildcard $(SRC)/*.cc))

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
WARNFLAGS += -Werror
ASFLAGS   := -D__ASSEMBLY__
CXXFLAGS  := -I$(INCLUDE) $(BASEFLAGS) $(WARNFLAGS)
CXXFLAGS  += -fno-exceptions -std=gnu++11
LDFLAGS   := $(BASEFLAGS)

.PHONY: default clean

default: $(BUILD)/$(IMG).img

clean:
	rm -rf $(BUILD)

$(BUILD)/$(IMG).img: $(BUILD)/$(IMG).elf
	$(OBJCOPY) $(BUILD)/$(IMG).elf -O binary $@

$(BUILD)/$(IMG).elf: $(OBJECTS) link-arm-eabi.ld
	$(CC) $(LDFLAGS) $(OBJECTS) -Tlink-arm-eabi.ld -lgcc -o $@

$(BUILD)/%.o: $(SRC)/%.S
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SRC)/%.cc $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CXXFLAGS) -c $< -o $@
