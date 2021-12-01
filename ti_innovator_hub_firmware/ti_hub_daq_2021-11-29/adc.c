/*
 * adc.c
 *
 *  Updated on: 18 Sep, 2021
 *      Author: William Jarratt
 */

#include "adc.h"
#include "macros.h"
#include "uart_ti82.h"

int adc_state = 0;
char adc_value_6 = 1;
char adc_value_8 = 1;
char adc_str[4] = {'a', 'a', 'p', '\0'};

extern volatile char tx1_done;
extern volatile char tx2_done;

void Init_ADC(void){

    // /4 sc_bit timer no_inv /1 mod rptS x xxxx 16x cont xx on xx enb scb
    // Control register 0 sets the clock source and operating mode.
    // The ADC is turned on and is disabled for configuration.
    // The operating mode is set to single channel continuous.
    //ADC14->CTL0 = 0x44040290 // Continuous single channel.
    ADC14->CTL0 = 0x44060290; // Continuous sequence.

    // xxxx no_sel x mem5 xxxxxxxxxx 12b unsigned on reg
    // Control register 1 sets the first memory channel to write to,
    // and sets the results to 12b unsigned. We will probably use 14b
    // in the future. Once the unsigned version works, we can move on
    // and see if its possible to get negative values.
    // Memory 5 is chosen as first memory channel. If sequence sample
    // is enabled then the memory channels starting at 5 are filled
    // until a memory channel designated as the end is encountered.
    //ADC14->CTL1 = 0x00050020;   // 12b
    ADC14->CTL1 = 0x00050000;   // 8b

    // xxxx xxxx xxxx xxxx x enb diffb x AVCC x xx A6
    // V+ and V- are set to 3.3V and 0V.
    // Memory channel 5 is mapped to A6. The end of sequence
    // bit is not set. If this were sequence sample, we would
    // map analog channel 8 (A8), to memory channel 6. Then we
    // would indicate memory channel 6 as the end of sequence.
    // This would lead to a sequence sample of A6 and A8.
    ADC14->MCTL[5] = 0x00000006;    // Map to A6, not end of sequence.
    ADC14->MCTL[6] = 0x00000088;    // Map to A8, is end of sequence.

    // Not currently using interrupts.
    ADC14->IER0 |= 0; //0x00000020;

    // Set enable and start conversions.
    ADC14->CTL0 |= 0x03;
}


void ADC_Process(void){


    switch(adc_state){
    case 0:
        if(tx1_done){
            adc_value_6 = ADC14->MEM[5];
            if(adc_value_6 == 0)
                ++adc_value_6;
            //HexToStr(adc_value_6);
            adc_state = 1;
        }
        break;
    case 1:
        if(tx1_done){
            adc_value_8 = ADC14->MEM[6];
            if(adc_value_8 == 0)
                ++adc_value_8;
            //HexToStr(adc_value_8);
            adc_state = 2;
        }
        break;
    case 2:
        if((Check_Command(1) && !Check_Command(3)) || Check_Command(2)){
            if(tx2_done && tx1_done){
                adc_str[0] = adc_value_6;
                adc_str[1] = adc_value_8;
                //TX_Debug(adc_str);
                adc_state = 0;
                //P2OUT  ^= GPIO_PIN6;
            }
            Clear_Command(1);
            Clear_Command(2);
        }
        break;
    default:
        break;
    }
}

/*
 *  As of right now, the interrupt is not needed. The ADC runs separate from the CPU,
 *  and when we need the values we simply poll the memory registers.
 */
void ADC14_IRQHandler(void){


}
