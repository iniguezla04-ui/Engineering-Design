#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "controller.h"
#include "clockConfig.h"
#include "printf.h"

void HAL_MspInit() {
    SystemClock_Config();    

}


