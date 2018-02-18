TOOLCHAIN ?= aarch64-none-elf
CC        := $(TOOLCHAIN)-g++
OBJCOPY   := $(TOOLCHAIN)-objcopy

IMG := kernel8

BUILD   := build
INCLUDE := ../include
SRC     := ../src
TEST    := .

INCLUDES := $(wildcard $(INCLUDE)/*.h)
INCLUDES += $(wildcard $(TEST)/*.h)

OBJECTS := $(patsubst $(SRC)/%.S, $(BUILD)/%.o, $(wildcard $(SRC)/*.S))
OBJECTS += $(patsubst $(SRC)/%.cc, $(BUILD)/%.o, $(wildcard $(SRC)/*.cc))
OBJECTS += $(patsubst $(TEST)/%.cc, $(BUILD)/test-%.o, $(wildcard $(TEST)/*.cc))

OBJECTS := $(filter-out $(BUILD)/main.o, $(OBJECTS))

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
ASFLAGS   := -D__ASSEMBLY__
CXXFLAGS  := -I$(TEST) -I$(INCLUDE) $(BASEFLAGS) $(WARNFLAGS)
CXXFLAGS  += -fno-exceptions -std=gnu++11
LDFLAGS   := $(BASEFLAGS)

.PHONY: default clean

default: $(BUILD)/$(IMG).img

clean:
	rm -rf $(BUILD)

$(BUILD)/$(IMG).img: $(BUILD)/$(IMG).elf
	$(OBJCOPY) $(BUILD)/$(IMG).elf -O binary $@

$(BUILD)/$(IMG).elf: $(OBJECTS) ../link-aarch64-elf.ld
	$(CC) $(LDFLAGS) $(OBJECTS) -T../link-aarch64-elf.ld -lgcc -o $@

$(BUILD)/%.o: $(SRC)/%.S
	@mkdir -p $(@D)
	$(CC) $(ASFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SRC)/%.cc $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CXXFLAGS) -c $< -o $@

$(BUILD)/test-%.o: $(TEST)/%.cc $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CXXFLAGS) -c $< -o $@
