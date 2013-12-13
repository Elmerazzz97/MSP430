#include <msp430.h>
#include <stdio.h>

void USART_Init()
{

	//Define the alternate functions for
	//P1.1 (UCA0RXD) and P1.2 (UCA0TXD)
	P1SEL |= BIT1 | BIT2;
	P1SEL2 |= BIT1 | BIT2;

	//Hold the usart in reset while we configure
	UCA0CTL1 |= UCSWRST;

	//No parity, LSB first, 8 bit data, 1 stop bit, UART mode
	UCA0CTL0 = 0;

	UCA0CTL1 |= UCSSEL_3 //SMCLCK
			| UCSWRST //Keep it reset
			;

	//Prescaler of 104. Baud rate will be 1MHz / 104 = 9615 baud7
	UCA0BR1 = 0;
	UCA0BR0 = 104;

	//No Modulation
	UCA0MCTL = 0;

	//Remove Reset
	UCA0CTL1 &=~ UCSWRST;

	 IE2 |= UCA0RXIE;                           // Enable USCI_A0 RX interrupt

}


void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !(IFG2 & UCA0TXIFG) );
	/* Put data into buffer, sends the data */
	UCA0TXBUF = data;
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(IFG2 & UCA0RXIFG) );
	/* Get and return received data from buffer */
	return UCA0RXBUF;
}

unsigned short USART_Received( void )
{
	/* Check if data has been received */
	return (IFG2 & UCA0RXIFG);
}

void USART_Send_String(char* s){
	//Use a while loop to transmit all characters
	while(*s){
		USART_Transmit(*s);
		s++;
		}
}

void USART_Send_Int(int x){
	char buf[7];
	sprintf(buf,"%d",x);
	USART_Send_String(buf);
}

void USART_NewLine(){
	USART_Transmit(13);//Return
	USART_Transmit(10);//Line Feed
}
