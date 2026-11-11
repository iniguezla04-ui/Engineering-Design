#include "controller.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "controller.h"
#include "printf.h"

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef osc = {0};
  RCC_ClkInitTypeDef clk = {0};

  osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  osc.HSEState       = RCC_HSE_ON;
  osc.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  osc.PLL.PLLState   = RCC_PLL_ON;
  osc.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  osc.PLL.PLLMUL     = RCC_PLL_MUL9;          // 8 MHz x 9 = 72 MHz
  if (HAL_RCC_OscConfig(&osc) != HAL_OK) { while(1); }

  clk.ClockType      = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                       RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
  clk.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  clk.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  clk.APB1CLKDivider = RCC_HCLK_DIV2;
  clk.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2) != HAL_OK) { while(1); }

  // ADC clock: 72 MHz / 6 = 12 MHz (max 14)
  RCC_PeriphCLKInitTypeDef pclk = {0};
  pclk.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  pclk.AdcClockSelection    = RCC_ADCPCLK2_DIV6;
  HAL_RCCEx_PeriphCLKConfig(&pclk);
}


