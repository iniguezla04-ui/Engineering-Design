/*
    * @file adc.h
    * @brief ADC driver header file
    *
    * This file contains the function declarations for initializing the ADC and reading values from ADC channels.
    *
    * @author Luis Aceves Iniguez
    * @date 2026-10-15
    *
*/



#ifndef ADC_H
#define ADC_H

#include <stdint.h>

/*
 * @return: None
 * @brief: Initialize the ADC
 */
static void adc_init(void);
/*
 * @param in8: Pointer to store the value read from ADC channel 8 (PB0)
 * @param in9: Pointer to store the value read from ADC channel 9 (PB1)
 * @return: None
 * @brief: Read values from ADC channels
 */
static void adc_read(uint32_t *in8, uint32_t *in9);
/*
 * @return: None
 * @brief: Error handler function to handle errors during ADC operations
*/
void Error_Handler(void);
#endif