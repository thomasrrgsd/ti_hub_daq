/*------------------------------------------------------------------------------
 *
 * main.c
 *
 * Description: This file contains the main function, which controls the overall
 * flow of the program. There is an endless loop that will make calls to other
 * functions when a task needs to be performed.
 *
 *----------------------------------------------------------------------------*/

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "adc.h"
#include "dma_ti82.h"
#include "fungen.h"
#include "i2c_ti82.h"
#include "interrupts.h"
#include "macros.h"
#include "multimeter.h"
#include "ports.h"
#include "timers.h"
#include "uart_ti82.h"
#include "logizer.h"

int main(void)
{

    __disable_irq();    // Disable all interrupts during device initialization

    Init_Ports();       // Initialize port pins
    Init_UART0();       // Initialize serial UART channel 0.
    Init_UART2();       // Initialize serial UART channel 2.
    Init_I2C();         // Initialize serial I2C channel 1.
    Init_ADC();         // Initialize 8-bit ADC.
    Init_TimerA0();     // Initialize timer A0.
    Init_TimerA1();     // Initialize timer A1.
    Init_DMA();         // Initialize DMA Channels.
    Init_FunGen();      // Initialize function generator.

    Init_Interrupts();  // Enable required interrupts and set priority.
    __enable_irq();     // Re-enable all interrupts for device operation.

    uint8_t i = 0;
    uint8_t ti82_state = 0;

    for(i = 0; i < 50; i++){    // Let hardware boot up, replace with Systick when able.
        msDelay(50);
    }

    Calibrate_Multimeter(DMM_CAL_VAL);    // Only needed for DMM.
    while(!Is_I2C_Ready());
    Configure_Multimeter(DMM_CONF_VAL);
    while(!Is_I2C_Ready());

    while(TRUE){

        // Receives commands from ESP via UART2 and updates command flags and values.
        // RX2_Command is no longer used since UART2 is now handled by DMA.
        //RX2_Command();

        // Due to bugs, a delay needed to be introduced. Ideally, this delay would
        // not need to exist.
        for(i = 0; i < 200; i++){
            msDelay(1);
        }

        // Checks if function generator has be changed by the user from the GUI.
        if(Check_Command(5)){
            Clear_Command(5);
            Update_Signal_Array();  // Disables function generator while updating array.
        }

        // Final Design Day code. Sends data in round-robin fashion.
        switch(ti82_state){
        case 0:
            if(Get_Command_Value(3) == 1){
                Dma_Ch2_ADC_Ch6_Get_Data();
                Clear_Command(1);
                ti82_state = 1;
            }
            else{
                ti82_state = 1;
            }
            break;
        case 1:
            if(Get_Command_Value(4) == 1){
                Dma_Ch3_ADC_Ch8_Get_Data();
                Clear_Command(1);
                ti82_state = 2;
            }
            else{
                ti82_state = 2;
            }
            break;
        case 2:
            if(Get_Command_Value(2) == 1){
                //for(i = 0; i < 100; i++){
                //    msDelay(1);
                //}
                Multimeter_Process();
                Clear_Command(1);
                ti82_state = 3;
            }
            else{
                ti82_state = 3;
            }
            break;
        case 3:
            if(UpdateLA()){
                Clear_Command(1);
                ti82_state = 0;
            }
            else{
                ti82_state = 0;
            }
            break;
        default:
            break;

        }

    }

}
