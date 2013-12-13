/*
 * Carlos Elmer Martinez
 * Project Name: Xbee Motor Controller
 *
 * This code uses UART and 6 buttons attached to interrupts
 * to send wireless commands through the Xbee to a receiving
 * Arduino connected to the motors.
 *
 */

#include <msp430.h> 
#include "usart.h"

// Delay Functions
#define delay_ms(x)		__delay_cycles((long) x* 1000)
#define delay_us(x)		__delay_cycles((long) x)

// Pin assignment
#define fwrdButton BIT3
#define backButton BIT4
#define leftButton BIT5
#define rightButton BIT6
#define stopButton BIT7
//#define LED0 BIT0



void main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    // Set DCO
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    USART_Init();				// Initializes UART

    P1OUT &= 0x00;               // Shut down everything
    P1DIR &= 0x00;

    // LED0 for debugging
    /*
    //P1DIR |= (LED0);
    //P1OUT |= (LED0);
    //delay_ms(1000);
    //P1OUT &= ~(LED0);
    */

    P1IE |= fwrdButton|backButton|leftButton|rightButton|stopButton;     	// Sets P1.3 -> P1.7 as interrupts
    //P1IFG &= ~(fwrdButton|backButton|leftButton|rightButton|stopButton);  // P1.3 -> P1.7 IFG cleared

    __bis_SR_register(LPM0_bits + GIE);	// Enters Low Power Mode 0 and enables global interrupts

    /* Debugging

    _enable_interrupts();
    while(1){
    	USART_Send_String("1");
    	delay_ms(500);
    	USART_Send_String("5");
		delay_ms(500);
	}*/

}


// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	_disable_interrupts(); // Disables interrupts
	delay_ms(100);	// Delays by 100 ms

	// Proceeds to check which button in the controller was pressed
	if(P1IFG & fwrdButton){
		USART_Send_String("1"); // Sends '1' if fwrdButton was pressed
		P1IFG &= ~fwrdButton;	// Clears IFG
	}
	if(P1IFG & backButton){
		USART_Send_String("2"); // Sends '2' if fwrdButton was pressed
		P1IFG &= ~backButton;	// Clears IFG
	}
	if(P1IFG & leftButton){
		USART_Send_String("3"); // Sends '3' if fwrdButton was pressed
		P1IFG &= ~leftButton;	// Clears IFG
	}
	if(P1IFG & rightButton){
		USART_Send_String("4"); // Sends '4' if fwrdButton was pressed
		P1IFG &= ~rightButton;	// Clears IFG
	}
	if(P1IFG & stopButton){
		USART_Send_String("S"); // Sends 'S' if fwrdButton was pressed
		P1IFG &= ~stopButton;	// Clears IFG
	}

	_enable_interrupts();	// Enables interrupts

}
