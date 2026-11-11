#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

void HAL_MspInit(void)
{
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_NOJTAG();     // keep SWD, free PB3/PB4/PA15
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *h)
{
  if (h->Instance == TIM1) {
    GPIO_InitTypeDef g = {0};
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    g.Pin = GPIO_PIN_8; g.Mode = GPIO_MODE_AF_PP; g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &g);
  }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *h)
{
  if (h->Instance == ADC1) {
    GPIO_InitTypeDef g = {0};
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    g.Pin = GPIO_PIN_0; g.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &g);
  }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *h)
{
  if (h->Instance == SPI1) {
    GPIO_InitTypeDef g = {0};
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    g.Pin = GPIO_PIN_5 | GPIO_PIN_7; g.Mode = GPIO_MODE_AF_PP; g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &g);
    g.Pin = GPIO_PIN_6; g.Mode = GPIO_MODE_INPUT; g.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &g);
  }
}



