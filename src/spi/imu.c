/*
 * ICM-42670-P driver over SPI1 (mode 0, CS on PA4).
 * Register addresses / values from the ICM-42670-P datasheet, bank 0.
 */
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "imu.h"
#include "peripheralInit.h"

extern SPI_HandleTypeDef hspi1;

#define CS_PORT GPIOA
#define CS_PIN  GPIO_PIN_4

#define REG_ACCEL_DATA_X1 0x0B   // ACCEL X/Y/Z then GYRO X/Y/Z, 12 bytes, big-endian
#define REG_PWR_MGMT0     0x1F
#define REG_WHO_AM_I      0x75
#define SPI_READ          0x80

#define PWR_GYRO_LN_ACCEL_LN 0x0F  // GYRO_MODE = 11 (low noise), ACCEL_MODE = 11 (low noise)

#define BURST_LEN (1 + 12)         // address byte + 12 data bytes

static uint8_t tx[BURST_LEN];
static uint8_t rx[BURST_LEN];

static volatile imu_sample_t latest;
static volatile uint32_t sample_count;
static volatile uint32_t error_count;

static inline void cs_low(void)  { HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET); }
static inline void cs_high(void) { HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET); }

static uint8_t read_reg_blocking(uint8_t reg)
{
    uint8_t t[2] = { reg | SPI_READ, 0x00 };
    uint8_t r[2] = { 0 };
    cs_low();
    if (HAL_SPI_TransmitReceive(&hspi1, t, r, 2, 10) != HAL_OK) Error_Handler();
    cs_high();
    return r[1];
}

static void write_reg_blocking(uint8_t reg, uint8_t val)
{
    uint8_t t[2] = { reg & 0x7F, val };
    cs_low();
    if (HAL_SPI_Transmit(&hspi1, t, 2, 10) != HAL_OK) Error_Handler();
    cs_high();
}

uint8_t imu_init(void)
{
    HAL_Delay(2);                                  // power-up time
    uint8_t id = read_reg_blocking(REG_WHO_AM_I);
    write_reg_blocking(REG_PWR_MGMT0, PWR_GYRO_LN_ACCEL_LN);
    HAL_Delay(50);                                 // gyro start-up (>= 45 ms)

    tx[0] = REG_ACCEL_DATA_X1 | SPI_READ;          // rest of tx[] stays 0 (dummy bytes)
    return id;
}

void imu_start_read(void)
{
    if (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) return;  // previous read still running
    cs_low();
    if (HAL_SPI_TransmitReceive_IT(&hspi1, tx, rx, BURST_LEN) != HAL_OK) {
        cs_high();
        error_count++;
    }
}

static inline int16_t be16(const uint8_t *p) { return (int16_t)((p[0] << 8) | p[1]); }

// Runs in SPI1 IRQ context when all BURST_LEN bytes are clocked
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *h)
{
    if (h->Instance != SPI1) return;
    cs_high();
    latest.ax = be16(&rx[1]);
    latest.ay = be16(&rx[3]);
    latest.az = be16(&rx[5]);
    latest.gx = be16(&rx[7]);
    latest.gy = be16(&rx[9]);
    latest.gz = be16(&rx[11]);
    sample_count++;
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *h)
{
    if (h->Instance != SPI1) return;
    cs_high();
    error_count++;
}

uint32_t imu_get(imu_sample_t *out)
{
    // Copy with interrupts masked so we never get half of an old sample
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    *out = latest;
    uint32_t n = sample_count;
    __set_PRIMASK(primask);
    return n;
}

uint32_t imu_errors(void) { return error_count; }
