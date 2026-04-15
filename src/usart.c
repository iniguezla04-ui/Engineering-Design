#include "usart.h"
#include "controller.h"

volatile ring_buffer_t ringBuffer;

void USART1_ISR(void) {
    
    if(bufferIsEmpty()) {
        USART1->CR1 &= 0x3FFF;
        return;
    }

    USART1->DR = ringBuffer.data[ringBuffer.head];
    if (ringBuffer.head == RING_BUFFER_SIZE)
        ringBuffer.head = 0;
    else
        ringBuffer.head ++;

}

void usart_init(void) {
    
    // 115,200 Baud @8Mhz ; USART1DIV = 0d4->34
    USART1->BRR |= 0x45;

    // Enable clock to uart
    RCC->APB2ENR |= 1 << 14;

    // Enable peripheral and TXE interrupts
    USART1->CR1 = (1 << 13) | (1 << 3);

    NVIC_EnableIRQ(37);



    return;
}

void putChar(char c) {
    if (bufferIsFull()) return;
    
// Disable interrupts for safety
    USART1->CR1 &= 0x3FFF;

    ringBuffer.data[ringBuffer.tail] = c;
    if (ringBuffer.tail == RING_BUFFER_SIZE - 1) 
        ringBuffer.tail = 0;
    else 
        ringBuffer.tail++;

    USART1->CR1 |= 1 << 13;

}

int bufferIsFull() {
    if (ringBuffer.head == 0) {
        if (ringBuffer.tail == RING_BUFFER_SIZE - 1) return 1; 
        else return 0;
    } else {
        if (ringBuffer.tail == ringBuffer.head - 1) return 1;
        else return 0;
    }
    
}

int bufferIsEmpty() {

    if (ringBuffer.head == ringBuffer.tail) return 1;
    else return 0;

}
