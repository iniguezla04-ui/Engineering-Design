#include "controller.h"
#include "printf.h"


int main() {

    usart_init();
    __enable_irq();

    while (1) {
        printf ("Hello\n");
       for (int i = 0; i < 100000000; i++ )
            __NOP();
    }
}
