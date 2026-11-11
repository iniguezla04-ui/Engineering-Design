#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "controller.h"
#include "clockConfig.h"
#include "peripheralInit.h"
#include "printf.h"

#define MAX 0xFFFFFFFUL

int main(void) {

    HAL_Init();
    SystemClock_Config();

    GPIO_Init();       
    TIM1_PWM_Init();
    ADC1_Init();
    SPI1_Init();

    while (1) {
        printf("fuck");
        HAL_Delay(1000);

    }
}
