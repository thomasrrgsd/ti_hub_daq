/*
 * interrupts.c
 *
 * Description: Handlers for PORT Interrupts. Uses NVIC to set priority and
 * enable each interrupt.
 *
 */


#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "macros.h"
#include "ports.h"
#include "uart_ti82.h"


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

// Port 1 interrupt handler. Deubgging with switches.
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
