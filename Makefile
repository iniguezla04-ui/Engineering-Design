TARGET=cortex-m3
CC=arm-none-eabi-gcc
CFLAGS=-mcpu=$(TARGET) -mthumb -mfloat-abi=soft -O0 -g3 -Wall -I include/
LDFLAGS=-T $(STARTUP_DIR)/STM32F103C8T6.ld --specs=nosys.specs -nostdlib -nolibc
SRCS=$(wildcard src/*.c)
OBJS=$(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRCS))
SRC_DIR=src/
BIN_DIR=build/bin
OBJ_DIR=build/obj
STARTUP_DIR=startup

all: $(BIN_DIR)/controller.hex

$(BIN_DIR)/controller.hex: $(BIN_DIR)/controller.elf
	arm-none-eabi-objcopy -O ihex $< $@

# link

$(BIN_DIR)/controller.elf: $(OBJS) $(HEADERS) $(BIN_DIR)/startup_ARMCM3.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ 

# compile

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN_DIR)/startup_ARMCM3.o: $(STARTUP_DIR)/startup_ARMCM3.c
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR): 
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)
.PHONY: clean all

clean: 
	rm -rf build/obj/* \
	rm -rf build/bin/*
