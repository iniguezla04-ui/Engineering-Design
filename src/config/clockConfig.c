#include "controller.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "controller.h"
#include "printf.h"

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef osc = {0};
  RCC_ClkInitTypeDef clk = {0};

  // The PCB has no crystal, so run from the internal 8 MHz RC oscillator (HSI).
  // The PLL only takes HSI / 2, so the maximum is 4 MHz x 16 = 64 MHz.
  osc.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  osc.HSIState            = RCC_HSI_ON;
  osc.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  osc.PLL.PLLState        = RCC_PLL_ON;
  osc.PLL.PLLSource       = RCC_PLLSOURCE_HSI_DIV2;
  osc.PLL.PLLMUL          = RCC_PLL_MUL16;    // 8 MHz / 2 x 16 = 64 MHz
  if (HAL_RCC_OscConfig(&osc) != HAL_OK) { while(1); }

  clk.ClockType      = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                       RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
  clk.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  clk.AHBCLKDivider  = RCC_SYSCLK_DIV1;     // HCLK  = 64 MHz
  clk.APB1CLKDivider = RCC_HCLK_DIV2;       // PCLK1 = 32 MHz (max 36); TIM2 clock = 2 x PCLK1 = 64 MHz
  clk.APB2CLKDivider = RCC_HCLK_DIV1;       // PCLK2 = 64 MHz: ADC1, SPI1, USART1
  if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_2) != HAL_OK) { while(1); }

  // ADC clock: 64 MHz / 6 = 10.67 MHz (max 14)
  RCC_PeriphCLKInitTypeDef pclk = {0};
  pclk.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  pclk.AdcClockSelection    = RCC_ADCPCLK2_DIV6;
  HAL_RCCEx_PeriphCLKConfig(&pclk);
}


