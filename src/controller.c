#include "controller.h"


int main() {

    usart_init();
    __enable_irq();

    while (1) {
        while(bufferIsFull());
        putChar('7');
        while(bufferIsFull());
        putChar('\n');
    }
}
