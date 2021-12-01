/*
 * interrupts.c
 *
 *  Created on: Apr 15, 2021
 *      Author: William Jarratt
 */


#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "macros.h"
#include "ports.h"

// Global variables
volatile char rx2_ring_buffer[BUFFER_UART]; // Ring buffer received from Wifi
volatile int  rx2_ring_write;               // Index for writing to ring buffer
volatile char tx2_process[BUFFER_UART]; // Buffer for transmitting to wifi
         int  tx2_index;                    // Index for sending out TX buffer
volatile char tx2_done;

volatile char rx1_ring_buffer[BUFFER_UART]; // Ring buffer received from Wifi
volatile int  rx1_ring_write;               // Index for writing to ring buffer
volatile char tx1_process[BUFFER_UART]; // Buffer for transmitting to wifi
         int  tx1_index;                    // Index for sending out TX buffer
volatile char tx1_done;


// Initialize interrupts by setting priority and mapping the correct interrupt vector.
void Init_Interrupts(void){

    // Set UART0 Interrupt
    NVIC_SetPriority(EUSCIA0_IRQn, 3);
    NVIC_ClearPendingIRQ(EUSCIA0_IRQn);
    NVIC_EnableIRQ(EUSCIA0_IRQn);

    // Set UART2 Interrupt
    NVIC_SetPriority(EUSCIA2_IRQn, 3);
    NVIC_ClearPendingIRQ(EUSCIA2_IRQn);
    NVIC_EnableIRQ(EUSCIA2_IRQn);

    // Set PORT1 Interrupt
    NVIC_SetPriority(PORT1_IRQn, 3);
    NVIC_ClearPendingIRQ(PORT1_IRQn);
    NVIC_EnableIRQ(PORT1_IRQn);

    NVIC_SetPriority(TA0_0_IRQn, 3);
    NVIC_ClearPendingIRQ(TA0_0_IRQn);
    NVIC_EnableIRQ(TA0_0_IRQn);

    NVIC_SetPriority(TA0_N_IRQn, 3);
    NVIC_ClearPendingIRQ(TA0_N_IRQn);
    NVIC_EnableIRQ(TA0_N_IRQn);

    NVIC_SetPriority(TA1_0_IRQn, 3);
    NVIC_ClearPendingIRQ(TA1_0_IRQn);
    NVIC_EnableIRQ(TA1_0_IRQn);

    NVIC_SetPriority(EUSCIB1_IRQn, 3);
    NVIC_ClearPendingIRQ(EUSCIB1_IRQn);
    NVIC_EnableIRQ(EUSCIB1_IRQn);
}

// Port 1 interrupt handler.
void PORT1_IRQHandler(void){

    // Code for port 1 driven interrupts
    // Currently does nothing.
    if (P1->IFG & SW1){
        //P2->OUT  |=  BLUE_LED;
        //while(!Is_I2C_Ready());
        //Read_Multimeter_Value(BUSV);
        //while(!Is_I2C_Ready());
        //Temp_Get_BusV();
        P1->IFG  &= ~SW1;
    }

    if (P1->IFG & SW2){
        //P2->OUT  &= ~BLUE_LED;
        P1->IFG  &= ~SW2;
    }

}

// UART0 interrupt handler.
// UART0 communicates with the PC.
void EUSCIA0_IRQHandler(void){

    char temp;

    switch(EUSCI_A0->IV){
    case NO_INTR:

        break;

    // -------------------------------------------------
    case RX_INTR:
        temp = EUSCI_A0->RXBUF;
        rx1_ring_buffer[rx1_ring_write++] = temp; // Place char in ring
        EUSCI_A0->TXBUF = temp; // Echo character to PC for debugging

        // Wrap the ring around the rosy
        if(rx1_ring_write >= BUFFER_UART){
            rx1_ring_write = RESET_ZERO;
        }
        break;

    // -------------------------------------------------
    case TX_INTR:
        EUSCI_A0->TXBUF = tx1_process[tx1_index++];   // Send out character pointed by index

        if(tx1_process[tx1_index] == '\0'){ // Reached end of string
            tx1_done = TRUE;
            EUSCI_A0->IE &= ~UCTXIE;        // Disable TX interrupt and reset index
            tx1_index     =  RESET_ZERO;
        }
        else if(tx1_index >= BUFFER_UART){  // Reached the end of buffer without NULL

            EUSCI_A0->IE &= ~UCTXIE;        // Disable TX interrupt and reset index
            tx1_index     =  RESET_ZERO;
        }
        break;

    default: break;
    }
}

// UART2 interrupt handler.
// UART2 communicates with the Wifi Module
void EUSCIA2_IRQHandler(void){

    char temp;

    switch(EUSCI_A2->IV){ // Checks interrupt vector to see if TX or RX caused flag.
    case NO_INTR:
        break;

    // -------------------------------------------------
    case RX_INTR:   // RX interrupt handle data
        temp = EUSCI_A2->RXBUF;
        rx2_ring_buffer[rx2_ring_write++] = temp; // Place char in ring
        //EUSCI_A0->TXBUF = temp; // Echo character to PC for debugging

        // Wrap the ring around the rosy
        if(rx2_ring_write >= sizeof(rx2_ring_buffer)){

            rx2_ring_write = RESET_ZERO;
        }
        break;

    // -------------------------------------------------
    case TX_INTR:   // TX interrupt, send out data
        EUSCI_A2->TXBUF = tx2_process[tx2_index++];   // Send out character pointed by index

        if(tx2_process[tx2_index] == '\0'){ // Reached end of string
            tx2_done = TRUE;
            EUSCI_A2->IE &= ~UCTXIE;        // Disable TX interrupt and reset index
            tx2_index     =  RESET_ZERO;
        }
        else if(tx2_index >= sizeof(tx2_process)){  // Reached the end of buffer without NULL

            EUSCI_A2->IE &= ~UCTXIE;        // Disable TX interrupt and reset index
            tx2_index     =  RESET_ZERO;
        }
        break;

    default: break; // Other interrupt source
    }
}
