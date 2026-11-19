#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "controller.h"
#include "clockConfig.h"
#include "peripheralInit.h"
#include "imu.h"
#include "printf.h"

#define PRINT_PERIOD_MS 100   // status line rate (10 Hz)
#define IMU_DIVIDER     10    // IMU read every 10 TIM2 ticks -> 100 Hz

extern TIM_HandleTypeDef htim2;
extern ADC_HandleTypeDef hadc1;

/* Written in ISRs, read in main() */
static volatile uint16_t adc_value;
static volatile uint32_t adc_count;
static volatile uint32_t tim_ticks;
static volatile uint16_t tim_latency_max;   // us from TIM2 update event to callback

int main(void) {

    HAL_Init();
    SystemClock_Config();

    GPIO_Init();
    TIM2_PWM_Init();
    ADC1_Init();
    SPI1_Init();
    usart_init();

    uint8_t id = imu_init();   // blocking SPI, done before the timer starts using SPI
    printf("\r\nboot: ICM-42670-P WHO_AM_I = 0x%02X (%s)\r\n",
           id, id == IMU_WHO_AM_I_VALUE ? "ok" : "UNEXPECTED");

    // Init only configures; the _IT starts turn on the interrupt sources
    if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK) Error_Handler();   // update IRQ @ 1 kHz
    if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK) Error_Handler();

    uint32_t next_print = HAL_GetTick();
    uint32_t last_ticks = 0, last_adc = 0, last_imu = 0;

    while (1) {
        if ((int32_t)(HAL_GetTick() - next_print) < 0) continue;
        next_print += PRINT_PERIOD_MS;

        // Snapshot ISR data with interrupts masked for a few cycles only
        __disable_irq();
        uint32_t ticks = tim_ticks;
        uint32_t nadc  = adc_count;
        uint16_t adc   = adc_value;
        uint16_t lat   = tim_latency_max;
        tim_latency_max = 0;
        __enable_irq();

        imu_sample_t s;
        uint32_t nimu = imu_get(&s);

        uint32_t ccr  = __HAL_TIM_GET_COMPARE(&htim2, TIM_CHANNEL_2);
        uint32_t duty = ccr * 1000U / (__HAL_TIM_GET_AUTORELOAD(&htim2) + 1U);   // per mille

        const uint32_t scale = 1000U / PRINT_PERIOD_MS;   // counts per period -> Hz
        printf("t=%lu tim=%luHz lat<=%uus | adc=%4u (%luHz) pwm=%lu.%lu%% | "
               "a=%6d %6d %6d g=%6d %6d %6d (%luHz err=%lu)\r\n",
               HAL_GetTick(), (ticks - last_ticks) * scale, lat,
               adc, (nadc - last_adc) * scale, duty / 10, duty % 10,
               s.ax, s.ay, s.az, s.gx, s.gy, s.gz,
               (nimu - last_imu) * scale, imu_errors());

        last_ticks = ticks;
        last_adc   = nadc;
        last_imu   = nimu;
    }
}

/* ---- HAL callbacks (run in interrupt context; keep them short) ---- */

// TIM2 update, 1 kHz: the control loop tick
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance != TIM2) return;

    // TIM2 counts 1 us ticks from 0 after the update event, so CNT = latency
    uint16_t lat = (uint16_t)__HAL_TIM_GET_COUNTER(htim);
    if (lat > tim_latency_max) tim_latency_max = lat;
    tim_ticks++;

    // PWM duty follows the last ADC reading. CCR2 is preloaded, so the new
    // value only takes effect at the next period boundary (no glitches).
    uint32_t ccr = ((uint32_t)adc_value * (__HAL_TIM_GET_AUTORELOAD(htim) + 1U)) / 4096U;
    __HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2, ccr);

    HAL_ADC_Start_IT(&hadc1);
    if (tim_ticks % IMU_DIVIDER == 0) imu_start_read();
}

// ADC end of conversion (PB0 / J1)
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance != ADC1) return;
    adc_value = (uint16_t)HAL_ADC_GetValue(hadc);
    adc_count++;
}
