#include "controller.h"


int main() {

    usart_init();
    __enable_irq();

    while (1) {
        printf ("Hello");
       for (int i = 0; i < 100000000; i++ )
            __NOP();
    }
}
