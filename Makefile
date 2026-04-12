TARGET=cortex-m3
CC=arm-none-eabi-gcc
CFLAGS=-c -mcpu=$(TARGET) -mthumb -mfloat-abi=soft -O0 -g3 -Wall
LDFLAGS=-T STM32F103C8T6.ld --specs=nosys.specs
SRCS=$(wildcard src/*.c)
OBJS=$(patsubst src/*.c, obj/*.o, $(SRCS))

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

controller.elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
