#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "controller.h"
#include "printf.h"

#define MAX 0xFFFFFFFUL

int main(void) {
    while (1) {
        printf("Hello, World!\n");
        for (int i = 0; i < MAX ; i++) {
            __NOP();
        }
    }
}
