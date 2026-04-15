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
    [53] = USART1_ISR
};
