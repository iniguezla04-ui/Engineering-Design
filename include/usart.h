void USART1_ISR(void);
void usart1_init(void);

#define RING_BUFFER_SIZE 16

typedef struct {
    char data[RING_BUFFER_SIZE];
    int head;
    int tail;
} ring_buffer_t;

int bufferIsFull(void);
int bufferIsEmpty(void);
