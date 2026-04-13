TARGET=cortex-m3
CC=arm-none-eabi-gcc
CFLAGS=-c -mcpu=$(TARGET) -mthumb -mfloat-abi=soft -O0 -g3 -Wall -I include/
LDFLAGS=-T STM32F103C8T6.ld --specs=nosys.specs -nostdlib
SRCS=$(wildcard src/*.c)
OBJS=$(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRCS))
SRC_DIR=src/
BIN_DIR=build/bin
OBJ_DIR=build/obj

all: $(BIN_DIR)/controller.elf

# link

$(BIN_DIR)/controller.elf: $(OBJS) $(HEADERS) | $(BIN_DIR)
	$(CC) $(LDFLAGS)  -o $@ $(OBJS)

# compile

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $@

startup_ARMCM3.o: startup_ARMCM3.S
	arm-none-eabi-as -mcpu=cortex-m3 -mthumb $< -o $@

$(OBJ_DIR): 
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)
.PHONY: clean all

clean: 
	rm -rf build/obj/* \
	rm -rf build/bin/*
