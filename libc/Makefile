TOOLCHAIN ?= aarch64-none-elf
CC        := $(TOOLCHAIN)-g++
AR        := $(TOOLCHAIN)-ar

LIB := libc

BUILD   := build
INCLUDE := include
SRC     := .

INCLUDES := $(wildcard $(INCLUDE)/*.h)

OBJECTS := $(patsubst $(SRC)/%.cc, $(BUILD)/%.o, $(wildcard $(SRC)/**/*.cc))

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
CXXFLAGS  := -I$(INCLUDE) $(BASEFLAGS) $(WARNFLAGS)
CXXFLAGS  += -fno-exceptions -std=gnu++11
LDFLAGS   := $(BASEFLAGS)

.PHONY: all clean

all: $(BUILD)/$(LIB).a

clean:
	rm -rf $(BUILD)

$(BUILD)/$(LIB).a: $(OBJECTS)
	$(AR) rcs $@ $(OBJECTS)

$(BUILD)/%.o: $(SRC)/%.cc $(INCLUDES)
	@mkdir -p $(@D)
	$(CC) $(CXXFLAGS) -c $< -o $@
