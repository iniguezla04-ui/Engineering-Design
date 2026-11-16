#ifndef USART_H
#define USART_H

void USART1_ISR(void);
void usart_init(void);

#define RING_BUFFER_SIZE 256

typedef struct {
    char data[RING_BUFFER_SIZE];
    int head;
    int tail;
} ring_buffer_t;

int bufferIsFull(void);
int bufferIsEmpty(void);
void putChar(char c);

#endif
