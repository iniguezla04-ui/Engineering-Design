TARGET=cortex-m3
CC=arm-none-eabi-gcc
CFLAGS=-mcpu=$(TARGET) -mthumb -mfloat-abi=soft -g3 -Wall \
	-I include/ -I $(STM32_HEADER_DIR) \
	-I $(HAL_DIR) -I $(CMSIS_DEV) -I $(CMSIS_CORE) -Ithird_party/printf \
	-DSTM32F103xB \
	-ffunction-sections -fdata-sections -ffreestanding

LDFLAGS=-T $(STARTUP_DIR)/STM32F103C8T6.ld --specs=nosys.specs \
	-Wl,--gc-sections \
	-nostdlib

SRC_DIR=src
BIN_DIR=build/bin
OBJ_DIR=build/obj
STARTUP_DIR=startup

HAL_DIR=third_party/stm32f1xx_hal_driver/Inc
HAL_SRC_DIR=third_party/stm32f1xx_hal_driver/Src
CMSIS_DEV=third_party/cmsis_device_f1/Include
CMSIS_CORE=third_party/cmsis_core/CMSIS/Core/Include
CMSIS_SRC_DIR=third_party/cmsis_device_f1/Source/Templates
STM32_HEADER_DIR=/home/jpmora403/tools/arm-gnu-toolchain/include/Include

SRCS=$(shell find $(SRC_DIR) -name '*.c')

HAL_MODULES = stm32f1xx_hal.c \
	stm32f1xx_hal_cortex.c \
	stm32f1xx_hal_gpio.c \
	stm32f1xx_hal_rcc.c \
	stm32f1xx_hal_rcc_ex.c \
	stm32f1xx_hal_dma.c \
	stm32f1xx_hal_adc.c \
	stm32f1xx_hal_adc_ex.c \
	stm32f1xx_hal_uart.c

EXT_SRCS = third_party/printf/printf.c \
	$(addprefix $(HAL_SRC_DIR)/, $(HAL_MODULES)) \
	$(CMSIS_SRC_DIR)/system_stm32f1xx.c

# Flatten all object file names into OBJ_DIR directly
OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.c=.o))) \
	$(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(EXT_SRCS)))

VPATH = $(shell find $(SRC_DIR) -type d)

all: $(BIN_DIR)/controller.hex success

success: $(BIN_DIR)/controller.hex
	@echo "========================================="
	@echo " 🎉 Build completed successfully!"
	@echo "========================================="

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

.PHONY: clean all success

clean:
	rm -rf build
