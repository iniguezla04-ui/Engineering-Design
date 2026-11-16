extern int main(void);
// Defined in linker script
extern unsigned int _stack;
extern unsigned int _idata;
extern unsigned int _data;
extern unsigned int _edata;
extern unsigned int _bss;
extern unsigned int _ebss;

// Defined in driver file
extern void USART1_ISR(void);
// Defined in src/config/it.c
extern void SysTick_Handler(void);
extern void ADC1_2_IRQHandler(void);
extern void TIM1_UP_IRQHandler(void);
extern void TIM1_CC_IRQHandler(void);
extern void SPI1_IRQHandler(void);

static void copy_data(void) {

    unsigned int* src_data_ptr = &_idata;
    unsigned int* dst_data_ptr = &_data;
    while (dst_data_ptr < &_edata) {
        *dst_data_ptr++ = *src_data_ptr++;
    }
}

static void clear_bss(void) {

    unsigned int* bss_ptr = &_bss;
    while (bss_ptr < &_ebss) {
        *bss_ptr++ = 0;
    }
}

void reset_isr(void) {
    copy_data();
    clear_bss();
    main();

    // Don't return from main
    while (1);
}
    
void isr_hardfault(void) {
    while (1);
}

#define PAD5    0,0,0,0,0
#define PAD10   PAD5,PAD5
#define IVT_ARRAY_SIZE 75U
typedef void(*isr_t)(void);
__attribute((used, section(".ivt")))
static const isr_t ivt[IVT_ARRAY_SIZE] = {
    
    [0] = (isr_t)&_stack,
    [1] = reset_isr,
    [3] = isr_hardfault,
    // Table index = 16 + IRQn
    [15] = SysTick_Handler,     // core exception, drives HAL_Delay()
    [34] = ADC1_2_IRQHandler,   // IRQ 18
    [41] = TIM1_UP_IRQHandler,  // IRQ 25
    [43] = TIM1_CC_IRQHandler,  // IRQ 27
    [51] = SPI1_IRQHandler,     // IRQ 35
    [53] = USART1_ISR           // IRQ 37
};
