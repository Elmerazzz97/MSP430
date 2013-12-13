#ifndef USART_H_
#define USART_H_

void USART_Init();
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
unsigned short USART_Received( void );
void USART_Send_String(char* s);
void USART_Send_Int(int x);
void USART_NewLine();

#endif /* USART_H_ */
