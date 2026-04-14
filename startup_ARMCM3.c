extern int main(void);
// Defined in linker script
extern unsigned int _stack;

void reset_isr(void) {
    main();

    // Don't return from main
    while (1);
}

void isr_hardfault(void) {
    while (1);
}

#define IVT_ARRAY_SIZE 75U
typedef void(*isr_t)(void);
__attribute((used, section(".ivt")))
static const isr_t ivt[IVT_ARRAY_SIZE] = {
    
    (isr_t) &_stack,
    reset_isr,
    0,
    isr_hardfault
};
