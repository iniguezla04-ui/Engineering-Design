#ifndef IMU_H
#define IMU_H

#include <stdint.h>

#define IMU_WHO_AM_I_VALUE 0x67   // ICM-42670-P

typedef struct {
    int16_t ax, ay, az;   // raw, default FS +-16 g    -> 2048 LSB/g
    int16_t gx, gy, gz;   // raw, default FS +-2000 dps -> 16.4 LSB/dps
} imu_sample_t;

/* Blocking setup over SPI1. Call once from main() before the timer starts
 * calling imu_start_read(). Returns the WHO_AM_I register. */
uint8_t imu_init(void);

/* Non-blocking burst read of accel + gyro (interrupt-driven). Safe to call
 * from an ISR; does nothing if the previous read is still in progress. */
void imu_start_read(void);

/* Copies the latest sample; returns the number of samples read so far. */
uint32_t imu_get(imu_sample_t *out);

/* Number of SPI errors (e.g. overrun) seen so far. */
uint32_t imu_errors(void);

#endif
