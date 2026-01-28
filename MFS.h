//#include "stm32f4xx_hal.h"
#include "mbed.h"

#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define beep_Pin GPIO_PIN_3
#define beep_GPIO_Port GPIOB
#define latch_Pin GPIO_PIN_5
#define latch_GPIO_Port GPIOB

#ifndef GPIO_SPEED_FREQ_VERY_HIGH 
#define GPIO_SPEED_FREQ_VERY_HIGH GPIO_SPEED_FREQ_HIGH
#endif

#ifndef GPIO_AF7_USART1
#define GPIO_AF7_USART1 1
#endif

class MFS
{   
    private:
    uint8_t dieAuswahl=0;
    uint8_t dieSegmente=0xFF;
    char seg7[10]={0xFC,0x60,0xDA, 0xF2, 0x66, 0xB6, 0xBE, 0xE0, 0xFE, 0xF6};
    USART_HandleTypeDef husart1;
    public:
    void operator=(unsigned int wert);
    operator int(void);
    MFS(void);
    void siebensegment(uint8_t wert);
    void siebensegment(uint8_t awert,uint8_t wert);
    void bcd(int wert);
    void bcd(uint8_t awert, int wert);
    void auswahl(uint8_t wert);
    private:
    void send(void);
};