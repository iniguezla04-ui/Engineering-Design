#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "controller.h"
#include "printf.h"

int main(void) {
    // 1. Initialize the HAL Library & System Clock
    HAL_Init();

    // 2. Enable GPIOC peripheral clock (if using PC13 for the built-in LED on Blue Pill)
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // 3. Configure PC13 as a Push-Pull Output
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    printf("System initialized successfully!\n");

    while (1) {
        // Toggle the LED pin state
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
        
        // Wait for 500 milliseconds (relies on HAL_GetTick() / SysTick timer)
        HAL_Delay(500);
        
        printf("Toggled LED...\n");
    }
}
