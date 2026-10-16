TARGET=cortex-m3
CC=arm-none-eabi-gcc
CFLAGS=-mcpu=$(TARGET) -mthumb -mfloat-abi=soft -O0 -g3 -Wall -I include/ -I $(STM32_HEADER_DIR) \
                      -I $(HAL_DIR) -I $(CMSIS_DEV) -I $(CMSIS_CORE) -Ithird_party/printf
LDFLAGS=-T $(STARTUP_DIR)/STM32F103C8T6.ld --specs=nosys.specs -nostdlib

SRC_DIR=src
BIN_DIR=build/bin
OBJ_DIR=build/obj
STARTUP_DIR=startup

HAL_DIR=third_party/stm32f1xx_hal_driver/Inc
CMSIS_DEV=third_party/cmsis_device_f1/Include
CMSIS_CORE=third_party/cmsis_core/CMSIS/Core/Include
STM32_HEADER_DIR=/home/jpmora403/tools/arm-gnu-toolchain/include/Include

# Source files
SRCS=$(wildcard $(SRC_DIR)/*.c)
EXT_SRCS=third_party/printf/printf.c

# Object files mapping (translates both local and external sources into OBJ_DIR)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS)) \
       $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(EXT_SRCS)))

all: $(BIN_DIR)/controller.hex success

$(BIN_DIR)/controller.hex: $(BIN_DIR)/controller.elf
				arm-none-eabi-objcopy -O ihex $< $@

simulate: $(BIN_DIR)/controller.elf
				qemu-system-arm -M stm32vldiscovery -kernel $< -serial stdio

simulate-debug: $(BIN_DIR)/controller.elf
				qemu-system-arm -M stm32vldiscovery -kernel $< -serial stdio -s -S

debug: $(BIN_DIR)/controller.elf
				gdb-multiarch build/bin/controller.elf \
				-ex "target remote :3333" \
				-ex "layout split" \
				-ex "break main" \
				-ex "continue"

# link (putting object files and startup object BEFORE LDFLAGS and -lgcc)
$(BIN_DIR)/controller.elf: $(OBJS) $(BIN_DIR)/startup_ARMCM3.o | $(BIN_DIR)
				$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) -lgcc

# compile local sources
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
				$(CC) -c $(CFLAGS) $< -o $@

# compile external sources (printf.c)
$(OBJ_DIR)/printf.o: third_party/printf/printf.c | $(OBJ_DIR)
				$(CC) -c $(CFLAGS) $< -o $@

$(BIN_DIR)/startup_ARMCM3.o: $(STARTUP_DIR)/startup_ARMCM3.c | $(BIN_DIR)
				$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR):
				mkdir -p $(OBJ_DIR)

$(BIN_DIR):
				mkdir -p $(BIN_DIR)

all: $(BIN_DIR)/controller.hex
	@echo "===================================="
	@echo " Build completed successfully"
	@echo "===================================="



.PHONY: clean all success

clean:
				rm -rf build
