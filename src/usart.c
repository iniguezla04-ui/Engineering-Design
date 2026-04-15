#include "usart.h"
#include "controller.h"

volatile ring_buffer_t ringBuffer;

void USART1_ISR(void) {
    
    if(bufferIsEmpty()) {
        USART1->CR1 &= ~(1 << 7);
        return;
    }

    USART1->DR = ringBuffer.data[ringBuffer.head];
    if (ringBuffer.head == RING_BUFFER_SIZE -1)
        ringBuffer.head = 0;
    else
        ringBuffer.head ++;

}

void usart_init(void) {
    RCC->APB2ENR |= (1 << 2) | (1 << 14);
    GPIOA->CRH &= ~(0xF << 4);  
    GPIOA->CRH |= (0xB << 4);   

    // 115,200 Baud @8Mhz ; USART1DIV = 0d4->34
    USART1->BRR |= 0x45;

    // Enable peripheral 
    USART1->CR1 = (1 << 13) | (1 << 3);

    NVIC_EnableIRQ(53);
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

    if (USART1->SR & (1 << 7))
        USART1_ISR();


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
