# Build for real Atari STE hardware/emulators with the m68k-atari-mint
# cross toolchain (https://tho-otto.de/crossmint.php or your distro's
# m68k-atari-mint-gcc package). libcmini or mintlib both work; nothing
# here depends on more than the trap-based helpers in src/tos.h.
#
#   make            -> build DINO.PRG
#   make check       -> syntax-check the source with a generic m68k-elf
#                        style compiler when the real cross toolchain
#                        isn't installed (does NOT produce a runnable
#                        Atari binary, just validates the C)
#   make run         -> build, then launch in Hatari if it's installed
#   make clean

CROSS   ?= m68k-atari-mint-gcc
CFLAGS  ?= -O2 -m68000 -Wall -Wextra -std=c99 -ffreestanding
LDFLAGS ?= -Wl,--traditional-format

SRC_DIR := src
BUILD_DIR := build
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET := DINO.PRG

.PHONY: all clean run check

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CROSS) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CROSS) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run: $(TARGET)
	hatari --tos auto $(TARGET)

# Fallback syntax check using any m68k GCC (e.g. gcc-m68k-linux-gnu)
# for development machines that don't have the Atari cross toolchain
# installed. Uses --register-prefix-optional since this codebase's
# inline asm (src/tos.h) is written in the plain Motorola register
# syntax (d0, sp, ...) that m68k-atari-mint-gcc expects.
CHECK_CC ?= m68k-linux-gnu-gcc
check:
	$(CHECK_CC) -std=c99 -ffreestanding -nostdlib -fno-pic -m68020 \
		-Wa,--register-prefix-optional -Wall -Wextra \
		-fsyntax-only $(SRCS)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
