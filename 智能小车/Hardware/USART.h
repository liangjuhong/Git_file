#ifndef __USART_H
#define __USART_H

void Usart_Init(void);
void USART_SendString(USART_TypeDef* USARTx,char *DataString);

#endif
