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
    // 1. Enable GPIOA clock (Bit 2 in APB2ENR)
RCC->APB2ENR |= (1 << 2);

// 2. Configure PA9 (TX) as Alternate Function Push-Pull
// Each pin in CRH takes 4 bits. PA9 is the second group (bits 4-7).
// We want: Mode = 11 (Output 50MHz), CNF = 10 (Alt Function PP) -> 0xB
GPIOA->CRH &= ~(0xF << 4);  // Clear current configuration for PA9
GPIOA->CRH |= (0xB << 4);   // Set CNF=10 and MODE=11 (0b1011 = 0xB)    
    // Enable clock to usart
    RCC->APB2ENR |= 1 << 14;

    // 115,200 Baud @8Mhz ; USART1DIV = 0d4->34
    USART1->BRR |= 0x45;

    // Enable peripheral 
    USART1->CR1 = (1 << 13) | (1 << 3);

    NVIC_EnableIRQ(37);



    return;
}

void putChar(char c) {
    if (bufferIsFull()) return;
    
// Disable interrupts for safety
    USART1->CR1 &= ~(1 << 7);

    ringBuffer.data[ringBuffer.tail] = c;
    if (ringBuffer.tail == RING_BUFFER_SIZE - 1) 
        ringBuffer.tail = 0;
    else 
        ringBuffer.tail++;

    USART1->CR1 |= (1 << 7);

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
