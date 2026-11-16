#include "stm32f1xx_hal.h"
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

    // BRR = PCLK2 / baud, rounded. Computed from the live clock so it stays
    // right if the clock changes. 64 MHz: 556 (0x22C) -> 115108 baud, -0.08%
    USART1->BRR = (HAL_RCC_GetPCLK2Freq() + 115200U / 2U) / 115200U;

    // Enable peripheral 
    USART1->CR1 = (1 << 13) | (1 << 3);
    NVIC_SetPriority(USART1_IRQn, 6);
    NVIC_EnableIRQ(USART1_IRQn);
    return;
}

// Called by printf for every character. Only call printf from main():
// it waits for room in the buffer, which would hang inside an ISR.
void _putchar(char character) {
    while (bufferIsFull()) { }   // the USART ISR frees space as it sends

    // Only this function moves tail and only the ISR moves head, so no lock is needed
    ringBuffer.data[ringBuffer.tail] = character;
    if (ringBuffer.tail == RING_BUFFER_SIZE - 1) 
        ringBuffer.tail = 0;
    else 
        ringBuffer.tail++;

    // Enable TXE interrupt. If the transmitter is idle (TXE already set) the
    // NVIC fires USART1_ISR right away; the ISR disables it when the buffer is empty.
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
