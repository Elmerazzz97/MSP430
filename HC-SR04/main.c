

//******************************************************************************
//   MSP430G2xx3 Demo - USCI_A0, 9600 UART Echo ISR, DCO SMCLK
//
//   Description: Echo a received character, RX ISR used. Normal mode is LPM0.
//   USCI_A0 RX interrupt triggers TX Echo.
//   Baud rate divider with 1MHz = 1MHz/9600 = ~104.2
//   ACLK = n/a, MCLK = SMCLK = CALxxx_1MHZ = 1MHz
//
//                MSP430G2xx3
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |     P1.2/UCA0TXD|------------>
//            |                 | 9600 - 8N1
//            |     P1.1/UCA0RXD|<------------
//
//   D. Dang
//   Texas Instruments Inc.
//   February 2011
//   Built with CCS Version 4.2.0 and IAR Embedded Workbench Version: 5.10
//******************************************************************************
#include  "msp430g2553.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "usart.h"

volatile unsigned int measure; //this is the difference in counts measured by the Ultrasonic timer

volatile unsigned int up=0; //helps the timer determine which edge

volatile unsigned int measure_1 = 0;
volatile unsigned int measure_2 = 0;

/*
 * Configuration for Trigger Pins - these drive the Ultrasonic device
 */
#define UltraPortOut P2OUT
#define UltraPortDirection P2DIR
#define UltraTrig BIT1
#define UltraEcho BIT0



void get_measure() {
    UltraPortOut |= UltraTrig;
    up = 1; //Next catch on Timer1A0 should be rising edge - helps with capture timer
    UltraPortOut &= ~UltraTrig;



/*
    up=1;
    P2OUT &= ~BIT1;
    __delay_cycles (2);
    P2OUT |=BIT1;
    __delay_cycles (10);
    P2OUT &= ~BIT1;
    __delay_cycles (100);
*/



}

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

  //***Timer1A? capture configuration
   //rising & falling edge + synchronous + P2.0 (CCI1A) + capture + capture/compare interrupt enable
   TA1CCTL0 |= CM_3 + CCIS_0 + CAP + CCIE;
   //select smclock for timer a source + make ta1ccr0 count continously up + no division
  TA1CTL |= TASSEL_2 + MC_2 + ID_0;



  //***Set up pins for Ultrasonic sensing
 UltraPortDirection = UltraTrig;
 UltraPortOut &= ~(UltraTrig);//turn off trigger pins to make sure they're in the correct state
 //Set P2.0 to pick up echo from the HC-SR04
 //Not using a #define element for this - it's tied to the timer
 P2SEL = UltraEcho;


  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
  DCOCTL = CALDCO_1MHZ;
  P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
  P1SEL2 = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
/*  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = 104;                            // 1MHz 9600
  UCA0BR1 = 0;                              // 1MHz 9600
  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
*/
  USART_Init();

  get_measure();

 // __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled


  while (1) {
      //_delay_cycles(50000);
       //get_measure();
	  USART_Send_String("1");
	  _delay_cycles(1000000);
	  USART_Transmit('2');
	  _delay_cycles(1000000);
	  USART_Send_String("3");
	  _delay_cycles(1000000);
	  USART_Send_String("5");
	  _delay_cycles(1000000);
	  USART_Transmit('4');
	  _delay_cycles(1000000);
  }
}






#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
}

//  Echo back RXed character, confirm TX buffer is ready first
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
  while (!(IFG2 & UCA0TXIFG));                // USCI_A0 TX buffer ready?
  char c = UCA0RXBUF;
  switch (c)  {
      case 'u': {
          //serialPrint("Hello World!\n\n");

    	  get_measure();
         // USART_Send_String("Distance ");
          /*USART_Send_Int(measure);
          USART_Send_String(" cm");
          USART_NewLine();*/

        }
  }


  //get_measure();
  //USART_Send_String("Distance ");
 // USART_Send_Int(measure);
  //USART_Send_String(" cm");
  //USART_NewLine();

//  UCA0TXBUF = c;                    // TX -> RXed character
}




//Timer1_A Capture
//P2.0 ends up triggering this timer
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer1A0(void)
{


   if(up) //is this the rising edge?
   {
      measure_1=TA1CCR0;  // take first time measure

      USART_Send_String("Measure 1:  ");
      USART_Send_Int(measure_1);
      USART_NewLine();

   }
   else //is this the falling edge?
   {
      measure_2=TA1CCR0; //take second time measure
      measure=(measure_2-measure_1)/58; // microseconds / 58 = centimeters

      USART_Send_String("Measure 2:  ");
      USART_Send_Int(measure_2);
      USART_NewLine();

      USART_Send_Int(measure);
      USART_Send_String(" cm");
      USART_NewLine();
      USART_NewLine();

   }
   up=!up; //if this was the rising edge, the next one will be a falling edge, and vice-versa
   TA1CTL &= ~TAIFG; //clear timer A interrupt flag, so the chip knows we handled the interrupt

}
