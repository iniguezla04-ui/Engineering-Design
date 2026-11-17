TARGET=cortex-m3
CC=arm-none-eabi-gcc
CFLAGS=-mcpu=$(TARGET) -mthumb -mfloat-abi=soft -g3 -Wall \
	-I include/ \
	-I $(HAL_DIR) -I $(CMSIS_DEV) -I $(CMSIS_CORE) -Ithird_party/printf \
	-DSTM32F103xB \
	-ffunction-sections -fdata-sections -ffreestanding

LDFLAGS=-T $(STARTUP_DIR)/STM32F103C8T6.ld --specs=nano.specs --specs=nosys.specs -Wl,--gc-sections -lc -lgcc

SRC_DIR=src
BIN_DIR=build/bin
OBJ_DIR=build/obj
STARTUP_DIR=startup

HAL_DIR=third_party/stm32f1xx_hal_driver/Inc
HAL_SRC_DIR=third_party/stm32f1xx_hal_driver/Src
CMSIS_DEV=third_party/cmsis_device_f1/Include
CMSIS_CORE=third_party/cmsis_core/CMSIS/Core/Include
CMSIS_SRC_DIR=third_party/cmsis_device_f1/Source/Templates

SRCS=$(shell find $(SRC_DIR) -name '*.c')

# Stop early with a clear message if the git submodules were never fetched
# (skipped for `make clean`, which doesn't need them)
SUBMODULE_FILES = third_party/printf/printf.c $(HAL_SRC_DIR)/stm32f1xx_hal.c \
	$(CMSIS_DEV)/stm32f1xx.h $(CMSIS_CORE)/core_cm3.h
ifneq ($(filter-out clean,$(or $(MAKECMDGOALS),all)),)
ifneq ($(words $(wildcard $(SUBMODULE_FILES))),$(words $(SUBMODULE_FILES)))
$(error third_party/ submodules are missing. Run: git submodule update --init)
endif
endif

# $(call need,<program>,<install hint>): fail with the hint if <program> isn't on PATH.
# The hint must not contain commas.
need = @command -v $(1) >/dev/null 2>&1 || { echo "Error: '$(1)' not found on PATH. $(2)"; exit 1; }

HAL_MODULES = stm32f1xx_hal.c \
	stm32f1xx_hal_cortex.c \
	stm32f1xx_hal_gpio.c \
	stm32f1xx_hal_rcc.c \
	stm32f1xx_hal_rcc_ex.c \
	stm32f1xx_hal_dma.c \
	stm32f1xx_hal_adc.c \
	stm32f1xx_hal_adc_ex.c \
	stm32f1xx_hal_uart.c \
	third_party/stm32f1xx_hal_driver/Src/stm32f1xx_hal_tim.c \
	third_party/stm32f1xx_hal_driver/Src/stm32f1xx_hal_tim_ex.c \
	third_party/stm32f1xx_hal_driver/Src/stm32f1xx_hal_spi.c 

EXT_SRCS = third_party/printf/printf.c \
	$(addprefix $(HAL_SRC_DIR)/, $(HAL_MODULES)) \
	$(CMSIS_SRC_DIR)/system_stm32f1xx.c

# Flatten all object file names into OBJ_DIR directly
OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.c=.o))) \
	$(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(EXT_SRCS)))

VPATH = $(shell find $(SRC_DIR) -type d)

SERIAL_PORT ?= /dev/ttyUSB0
FLASH_BAUD ?= 115200
MONITOR_BAUD ?= 115200

all: $(BIN_DIR)/controller.hex success

success: $(BIN_DIR)/controller.hex
	@echo "========================================="
	@echo " 🎉 Build completed successfully!"
	@echo "========================================="

$(BIN_DIR)/controller.hex: $(BIN_DIR)/controller.elf
	arm-none-eabi-objcopy -O ihex $< $@

simulate: $(BIN_DIR)/controller.elf
	$(call need,qemu-system-arm,Install it: sudo apt install qemu-system-arm)
	qemu-system-arm -M stm32vldiscovery -kernel $< -serial stdio

simulate-debug: $(BIN_DIR)/controller.elf
	$(call need,qemu-system-arm,Install it: sudo apt install qemu-system-arm)
	qemu-system-arm -M stm32vldiscovery -kernel $< -serial stdio -s -S

debug: $(BIN_DIR)/controller.elf
	$(call need,gdb-multiarch,Install it: sudo apt install gdb-multiarch)
	gdb-multiarch build/bin/controller.elf \
	-ex "target remote :3333" \
	-ex "layout split" \
	-ex "break main" \
	-ex "continue"

# Check for the compiler once, before the first object is built
$(OBJS) $(BIN_DIR)/startup_ARMCM3.o: | check-toolchain

check-toolchain:
	$(call need,$(CC),Install it with: sudo apt install gcc-arm-none-eabi  -- or download the Arm GNU Toolchain and add its bin/ folder to PATH in ~/.bashrc)

$(BIN_DIR)/controller.elf: $(OBJS) $(BIN_DIR)/startup_ARMCM3.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) -lgcc

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/printf.o: third_party/printf/printf.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(HAL_SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: $(CMSIS_SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN_DIR)/startup_ARMCM3.o: $(STARTUP_DIR)/startup_ARMCM3.c | $(BIN_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

.PHONY: clean all success flash monitor check-toolchain simulate simulate-debug debug

flash: $(BIN_DIR)/controller.hex
	$(call need,stm32flash,Install it: sudo apt install stm32flash)
	stm32flash -w $< -v -g 0x08000000 -b $(FLASH_BAUD) $(SERIAL_PORT)

clean:
	rm -rf build

# Open a serial terminal on the board's UART. Exit with Ctrl-A then Ctrl-X.
# WSL: attach the adapter first from an admin PowerShell on Windows:
#   usbipd list                       (find the adapter's BUSID)
#   usbipd bind --busid <BUSID>       (once per adapter)
#   usbipd attach --wsl --busid <BUSID>   (after each replug)
# Override the port or baud: make monitor SERIAL_PORT=/dev/ttyACM0 MONITOR_BAUD=9600
monitor:
	$(call need,picocom,Install it: sudo apt install picocom)
	@if [ ! -e $(SERIAL_PORT) ]; then \
		echo "$(SERIAL_PORT) not found. Ports present:"; \
		ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null || echo "  (none)"; \
		echo "On WSL, attach the adapter with usbipd (see comment above the monitor target)."; \
		exit 1; \
	fi
	picocom -b $(MONITOR_BAUD) $(SERIAL_PORT) || \
		(echo "If permission was denied: sudo usermod -aG dialout $$USER, then restart WSL"; exit 1)
