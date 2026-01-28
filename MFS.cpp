#include "MFS.h"

MFS::MFS(void)
    {
    HAL_Init();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
        /* GPIO Ports Clock Enable */
      __HAL_RCC_GPIOC_CLK_ENABLE();
      //__HAL_RCC_GPIOH_CLK_ENABLE(); //F103
      __HAL_RCC_GPIOA_CLK_ENABLE();
      __HAL_RCC_GPIOB_CLK_ENABLE();
      __HAL_RCC_USART1_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, beep_Pin|latch_Pin, GPIO_PIN_SET);
  
/*Configure GPIO pin : beep_Pin */
  GPIO_InitStruct.Pin = beep_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(beep_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : latch_Pin */
  GPIO_InitStruct.Pin = latch_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  

  
  HAL_GPIO_Init(latch_GPIO_Port, &GPIO_InitStruct);  
 
     /**USART1 GPIO Configuration
    PA8     ------> USART1_CK
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; //F103

    #ifndef __STM32F1xx_HAL_H 
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1; //F103
    #endif
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
      husart1.Instance = USART1;
      husart1.Init.BaudRate = 115000;
      husart1.Init.WordLength = USART_WORDLENGTH_8B;
      husart1.Init.StopBits = USART_STOPBITS_1;
      husart1.Init.Parity = USART_PARITY_NONE;
      husart1.Init.Mode = USART_MODE_TX_RX;
      husart1.Init.CLKPolarity = USART_POLARITY_LOW;
      husart1.Init.CLKPhase = USART_PHASE_1EDGE;
      husart1.Init.CLKLastBit = USART_LASTBIT_ENABLE;
      if (HAL_USART_Init(&husart1) != HAL_OK)
      {
        //Error_Handler();
        HAL_GPIO_WritePin(GPIOB,beep_Pin,GPIO_PIN_RESET);
      }
   };

    void MFS::send(void)
    {
            HAL_GPIO_WritePin(GPIOB, latch_Pin, GPIO_PIN_RESET);
            HAL_USART_Transmit(&husart1,&dieSegmente,1,100);
            HAL_USART_Transmit(&husart1,&dieAuswahl,1,100);
            HAL_GPIO_WritePin(GPIOB, latch_Pin, GPIO_PIN_SET);
    }
    void MFS::siebensegment(uint8_t wert)
    {
        dieSegmente=~wert;
        send();
    }
    void MFS::siebensegment(uint8_t awert,uint8_t wert)
    {
        dieSegmente=~wert;
        dieAuswahl=awert;
        send();
    }
    void MFS::bcd(int wert)
    {
        dieSegmente=~seg7[wert];
        send();
    }
    
    void MFS::bcd(uint8_t awert, int wert)
    {
        dieSegmente=~seg7[wert];
        dieAuswahl=awert;
        send();
    }    
    void MFS::auswahl(uint8_t wert)
    {
        dieAuswahl=wert;
        send();
    }
    
    void MFS::operator=(unsigned int wert)
    {
        char hilf=0, hilf2=wert;
        for (int i=0;i<8;i++)
        {
         hilf=(hilf<<1)+hilf2%2; 
         hilf2=hilf2/2;  
        }
        dieSegmente=(~hilf)&0xFF;
        dieAuswahl=(wert/0x100);
        send();
    }
    
    MFS::operator int(void)
    {
        char hilf=0, hilf2=~dieSegmente;
        for (int i=0;i<8;i++)
        {
         hilf=(hilf<<1)+hilf2%2; 
         hilf2=hilf2/2;  
        }        
        return dieAuswahl*0x100+hilf;
    }