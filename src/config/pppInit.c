#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "pppInit.h"

TIM_HandleTypeDef  htim1;
ADC_HandleTypeDef  hadc1;
UART_HandleTypeDef huart1;
SPI_HandleTypeDef  hspi1;



void TIM1_PWM_Init(void)
{
  TIM_OC_InitTypeDef oc = {0};
  htim1.Instance               = TIM1;
  htim1.Init.Prescaler         = 71;            // 1 MHz tick
  htim1.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim1.Init.Period            = 999;           // 1 kHz
  htim1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) Error_Handler();

  oc.OCMode     = TIM_OCMODE_PWM1;
  oc.Pulse      = 500;                          // 50% duty
  oc.OCPolarity = TIM_OCPOLARITY_HIGH;
  oc.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &oc, TIM_CHANNEL_1) != HAL_OK) Error_Handler();
}

void ADC1_Init(void)
{
  ADC_ChannelConfTypeDef ch = {0};
  hadc1.Instance                   = ADC1;
  hadc1.Init.ScanConvMode          = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode    = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion       = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK) Error_Handler();

  ch.Channel      = ADC_CHANNEL_0;               // PA0
  ch.Rank         = ADC_REGULAR_RANK_1;
  ch.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &ch) != HAL_OK) Error_Handler();
}

void SPI1_Init(void)
{
  hspi1.Instance               = SPI1;
  hspi1.Init.Mode              = SPI_MODE_MASTER;
  hspi1.Init.Direction         = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize          = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity       = SPI_POLARITY_LOW;   // match your device
  hspi1.Init.CLKPhase          = SPI_PHASE_1EDGE;    // match your device
  hspi1.Init.NSS               = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;  // 9 MHz
  hspi1.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode            = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK) Error_Handler();
}

void GPIO_Init(void)
{
  GPIO_InitTypeDef g = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // CS high first
  g.Pin = GPIO_PIN_4; g.Mode = GPIO_MODE_OUTPUT_PP; g.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &g);
}

void Error_Handler() {
    while (1);
}
