#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc_ex.h"

static ADC_HandleTypeDef hadc1;

static void adc_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    gpio.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOB, &gpio);

    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.NbrOfDiscConversion = 1;
    hadc1.Init.NbrOfConversion = 2;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;

    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    ADC_ChannelConfTypeDef channel = {0};
    channel.Channel = ADC_CHANNEL_8;
    channel.Rank = ADC_REGULAR_RANK_1;
    channel.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    if (HAL_ADC_ConfigChannel(&hadc1, &channel) != HAL_OK) {
        Error_Handler();
    }

    channel.Channel = ADC_CHANNEL_9;
    channel.Rank = ADC_REGULAR_RANK_2;
    if (HAL_ADC_ConfigChannel(&hadc1, &channel) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK) {
        Error_Handler();
    }
}

static void adc_read(uint32_t *in8, uint32_t *in9)
{
    if (HAL_ADC_Start(&hadc1) != HAL_OK ||
        HAL_ADC_PollForConversion(&hadc1, 100) != HAL_OK) {
        Error_Handler();
    }
    *in8 = HAL_ADC_GetValue(&hadc1);  // Rank 1: ADC1_IN8 / PB0

    if (HAL_ADC_PollForConversion(&hadc1, 100) != HAL_OK) {
        Error_Handler();
    }
    *in9 = HAL_ADC_GetValue(&hadc1);  // Rank 2: ADC1_IN9 / PB1

    if (HAL_ADC_Stop(&hadc1) != HAL_OK) {
        Error_Handler();
    }
}