#include "controller.h"


int main() {

    usart_init();

    while (1) {
        putChar('7');
        putChar('\n');
    }
}
