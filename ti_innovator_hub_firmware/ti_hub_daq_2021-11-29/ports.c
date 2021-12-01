/*
 * ports.c
 *
 *  Created on: Mar 18, 2021
 *      Author: William Jarratt
 */

#include "macros.h"
#include "ports.h"


/*-----------------------------------------------------------------------------
 * Function name: Init_Ports
 *
 * Description:
 *
 * Passed : none
 * Locals: none
 * Returned: none
 * Globals:  none
 *
 *---------------------------------------------------------------------------*/
void Init_Ports(void){

  Init_Port_1();
  Init_Port_2();
  Init_Port_3();
  Init_Port_4();
  Init_Port_5();
  Init_Port_6();
  Init_Port_7();

}


void Init_Port_1(void){

    // Port 1 Pin 0 is configured to output to LED1.
    // This is achieved by a jumper on the MSP432 main board.
    P1->SEL0 &= ~LED1;
    P1->SEL1 &= ~LED1;
    P1->OUT  &= ~LED1;
    P1->DIR  |=  LED1;

    // P1.1 is configured for switch 1 input GPIO
    P1->SEL0 &= ~SW1;       // GPIO selected
    P1->SEL1 &= ~SW1;
    P1->OUT  |=  SW1;       // Pull-up
    P1->DIR  &= ~SW1;       // Input
    P1->REN  |=  SW1;       // Pull U/D enabled
    P1->IES  |=  SW1;       // Interrupt on Hi -> Lo
    P1->IFG  |=  SW1;       // Clear previous interrupts
    P1->IE   |=  SW1;       // Enable interrupt

    // P1.2 is configured for UART Rx
    P1->SEL0 |=  UCA0RXD;
    P1->SEL1 &= ~UCA0RXD;

    // P1.3 is configured for UART Tx
    P1->SEL0 |=  UCA0TXD;
    P1->SEL1 &= ~UCA0TXD;

    // P1.4 is configured for switch 2 input GPIO.
    P1->SEL0 &= ~SW2;       // GPIO selected
    P1->SEL1 &= ~SW2;
    P1->OUT  |=  SW2;       // Pull-up
    P1->DIR  &= ~SW2;       // Input
    P1->REN  |=  SW2;       // Pull U/D enabled
    P1->IES  |=  SW2;       // Interrupt on Hi -> Lo
    P1->IFG  |=  SW2;       // Clear previous interrupts
    P1->IE   |=  SW2;       // Enable interrupt

    // P1.5 is configured for Logic Analyzer Channel 3.
    P1->SEL0 &= ~GPIO_PIN5;       // GPIO selected
    P1->SEL1 &= ~GPIO_PIN5;
    P1->OUT  &= ~GPIO_PIN5;       // Pull-down
    P1->DIR  &= ~GPIO_PIN5;       // Input
    P1->REN  |=  GPIO_PIN5;       // Pull U/D enabled
    //P1->IES  |=  GPIO_PIN5;       // Interrupt on Hi -> Lo
    //P1->IFG  |=  GPIO_PIN5;       // Clear previous interrupts
    //P1->IE   |=  GPIO_PIN5;       // Enable interrupt

    // P1.6 is configured for Logic Analyzer Channel 2.
    P1->SEL0 &= ~GPIO_PIN6;       // GPIO selected
    P1->SEL1 &= ~GPIO_PIN6;
    P1->OUT  &= ~GPIO_PIN6;       // Pull-down
    P1->DIR  &= ~GPIO_PIN6;       // Input
    P1->REN  |=  GPIO_PIN6;       // Pull U/D enabled
    //P1->IES  |=  GPIO_PIN6;       // Interrupt on Hi -> Lo
    //P1->IFG  |=  GPIO_PIN6;       // Clear previous interrupts
    //P1->IE   |=  GPIO_PIN6;       // Enable interrupt

    // P1.7 is configured for Logic Analyzer Channel 1.
    P1->SEL0 &= ~GPIO_PIN7;       // GPIO selected
    P1->SEL1 &= ~GPIO_PIN7;
    P1->OUT  &= ~GPIO_PIN7;       // Pull-down
    P1->DIR  &= ~GPIO_PIN7;       // Input
    P1->REN  |=  GPIO_PIN7;       // Pull U/D enabled
    //P1->IES  |=  GPIO_PIN7;       // Interrupt on Hi -> Lo
    //P1->IFG  |=  GPIO_PIN7;       // Clear previous interrupts
    //P1->IE   |=  GPIO_PIN7;       // Enable interrupt

}


void Init_Port_2(void){

    P2SEL0 &= ~GPIO_PIN0;
    P2SEL1 &= ~GPIO_PIN0;
    P2OUT  &= ~GPIO_PIN0;
    P2DIR  |=  GPIO_PIN0;

    P2SEL0 &= ~GPIO_PIN1;
    P2SEL1 &= ~GPIO_PIN1;
    P2OUT  &= ~GPIO_PIN1;
    P2DIR  |=  GPIO_PIN1;

    P2->SEL0 &= ~BLUE_LED;
    P2->SEL1 &= ~BLUE_LED;
    P2->OUT  &= ~BLUE_LED;
    P2->DIR  |=  BLUE_LED;

    P2SEL0 &= ~GPIO_PIN6;
    P2SEL1 &= ~GPIO_PIN6;
    P2OUT  &= ~GPIO_PIN6;
    P2DIR  |=  GPIO_PIN6;

    // P2.7 is configured for Logic Analyzer Channel 8.
    P2->SEL0 &= ~GPIO_PIN7;       // GPIO selected
    P2->SEL1 &= ~GPIO_PIN7;
    P2->OUT  &= ~GPIO_PIN7;       // Pull-down
    P2->DIR  &= ~GPIO_PIN7;       // Input
    P2->REN  |=  GPIO_PIN7;       // Pull U/D enabled
    //P2->IES  |=  GPIO_PIN7;       // Interrupt on Hi -> Lo
    //P2->IFG  |=  GPIO_PIN7;       // Clear previous interrupts
    //P2->IE   |=  GPIO_PIN7;       // Enable interrupt
}


void Init_Port_3(void){

    // P3.0 is configured for Logic Analyzer Channel 4.
    P3->SEL0 &= ~GPIO_PIN0;       // GPIO selected
    P3->SEL1 &= ~GPIO_PIN0;
    P3->OUT  &= ~GPIO_PIN0;       // Pull-down
    P3->DIR  &= ~GPIO_PIN0;       // Input
    P3->REN  |=  GPIO_PIN0;       // Pull U/D enabled
    //P3->IES  |=  GPIO_PIN0;       // Interrupt on Hi -> Lo
    //P3->IFG  |=  GPIO_PIN0;       // Clear previous interrupts
    //P3->IE   |=  GPIO_PIN0;       // Enable interrupt


    // Port 3 Pin 2 is configured for UART Rx
    P3->SEL0 |=  UCA2RXD;
    P3->SEL1 &= ~UCA2RXD;

    // Port 3 Pin 3 is configured for UART Tx
    P3->SEL0 |=  UCA2TXD;
    P3->SEL1 &= ~UCA2TXD;
}

void Init_Port_4(void){

    // Port 4 Pin 3 normally set for GPIO, use for MCLK debugging
    //P4->SEL0 |=  GPIO_PIN3;
    //P4->SEL1 &= ~GPIO_PIN3;
    //P4->DIR  |=  GPIO_PIN3;
    P4->SELC &= ~GPIO_PIN3;
    P4->DIR  &= ~GPIO_PIN3;

    // Port 4 Pin 5 is ADC for channel A8
    P4->SELC |= V_CH_A8;

    // Port 4 Pin 7 is ADC for channel A6
    P4->SELC |= V_CH_A6;

    // Port 7 pin 0 is not mapped to the connectors. Used only for SMCLK debugging.
    //P7->SEL0 |= GPIO_PIN0;
    //P7->SEL1 &= ~GPIO_PIN0;
    //P7->DIR |= GPIO_PIN0;
}

void Init_Port_5(void){

    // P5.0 is configured for Logic Analyzer Channel 5.
    P5->SEL0 &= ~GPIO_PIN0;       // GPIO selected
    P5->SEL1 &= ~GPIO_PIN0;
    P5->OUT  &= ~GPIO_PIN0;       // Pull-down
    P5->DIR  &= ~GPIO_PIN0;       // Input
    P5->REN  |=  GPIO_PIN0;       // Pull U/D enabled
    //P5->IES  |=  GPIO_PIN0;       // Interrupt on Hi -> Lo
    //P5->IFG  |=  GPIO_PIN0;       // Clear previous interrupts
    //P5->IE   |=  GPIO_PIN0;       // Enable interrupt

    // P5.1 is configured for Logic Analyzer Channel 7.
    P5->SEL0 &= ~GPIO_PIN1;       // GPIO selected
    P5->SEL1 &= ~GPIO_PIN1;
    P5->OUT  &= ~GPIO_PIN1;       // Pull-down
    P5->DIR  &= ~GPIO_PIN1;       // Input
    P5->REN  |=  GPIO_PIN1;       // Pull U/D enabled
    //P5->IES  |=  GPIO_PIN1;       // Interrupt on Hi -> Lo
    //P5->IFG  |=  GPIO_PIN1;       // Clear previous interrupts
    //P5->IE   |=  GPIO_PIN1;       // Enable interrupt

    // P5.2 is configured for Logic Analyzer Channel 6.
    P5->SEL0 &= ~GPIO_PIN2;       // GPIO selected
    P5->SEL1 &= ~GPIO_PIN2;
    P5->OUT  &= ~GPIO_PIN2;       // Pull-down
    P5->DIR  &= ~GPIO_PIN2;       // Input
    P5->REN  |=  GPIO_PIN2;       // Pull U/D enabled
    //P5->IES  |=  GPIO_PIN2;       // Interrupt on Hi -> Lo
    //P5->IFG  |=  GPIO_PIN2;       // Clear previous interrupts
    //P5->IE   |=  GPIO_PIN2;       // Enable interrupt

    // Port 5 Pin 4 is ADC for channel A1
    P5->SELC |=  V_CH_A1;

    // Port 5 Pin 5 is ADC for channel A0
    P5->SELC |=  V_CH_A0;
}

void Init_Port_6(void){

    // I2C port configuration happens while I2C reset is held in I2C init.
    // Port 6 Pin 4 is I2C Signal Data
    /*P6->SEL0 |=  UCB1SDA;
    P6->SEL1 &= ~UCB1SDA;

    // Port 6 Pin 5 is I2C Signal Clock
    P6->SEL0 |=  UCB1SCL;
    P6->SEL1 &= ~UCB1SCL;*/
}

void Init_Port_7(void){ // Port 7 utilizes Port Mapping and must be configured as such.

    // Port 7 Pin 0 is used to debug SMCLK.
    /*PMAP->KEYID = 0x2D52;
    P7MAP->PMAP_REGISTER0 = PM_SMCLK;
    P7->SEL0 |=  GPIO_PIN0;
    P7->SEL1 &= ~GPIO_PIN0;
    P7->DIR  |=  GPIO_PIN0;
    PMAP->CTL = 1;
    PMAP->KEYID = 0;*/
}

