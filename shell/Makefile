TOOLCHAIN ?= aarch64-none-elf
CC        := $(TOOLCHAIN)-g++
OBJCOPY   := $(TOOLCHAIN)-objcopy
OBJDUMP   := $(TOOLCHAIN)-objdump

IMG := shell

BUILD   := build
INCLUDE := include
LIBC    := ../libc/build
SRC     := .

INCLUDES := $(wildcard $(INCLUDE)/*.h)

OBJECTS := $(patsubst $(SRC)/%.cc, $(BUILD)/%.o, $(wildcard $(SRC)/*/*.cc))
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
CXXFLAGS  := $(BASEFLAGS) $(WARNFLAGS) $(INCFLAGS)
CXXFLAGS  += -fno-exceptions -std=gnu++11
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

$(BUILD)/%.o: $(SRC)/%.cc $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CXXFLAGS) -c $< -o $@
