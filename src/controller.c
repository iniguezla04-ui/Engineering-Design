#include "controller.h"


int main() {

    usart_init();
    __enable_irq();

    while (1) {
        putChar('7');
        putChar('\n');
    }
}
