/*
 * logizer.c
 *
 *  Created on: Nov 9, 2021
 *      Author: willi
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "dma_ti82.h"
#include "macros.h"
#include "logizer.h"

uint8_t logicChar = 0;

uint8_t LA_blob[2] = {3, 0};
uint16_t LA_len = 2;

uint8_t UpdateLA(void){

    uint8_t nextChar = 0;

    //Channel 1
    nextChar |= (P1->IN & GPIO_PIN7) >> 7;

    //Channel 2
    nextChar |= (P1->IN & GPIO_PIN6) >> 5;

    //Channel 3
    nextChar |= (P1->IN & GPIO_PIN5) >> 3;

    //Channel 4
    nextChar |= (P3->IN & GPIO_PIN0) << 3;

    //Channel 5
    nextChar |= (P5->IN & GPIO_PIN0) << 4;

    //Channel 6
    nextChar |= (P5->IN & GPIO_PIN2) << 3;

    //Channel 7
    nextChar |= (P5->IN & GPIO_PIN1) << 5;

    //Channel 8
    nextChar |= (P2->IN & GPIO_PIN7);

    if(nextChar != logicChar){
        // Send byte packet.
        LA_blob[1] = nextChar;
        Dma_Ch4_Uart2_Send_String(LA_blob, LA_len);
        //while(Dma_Ch4_Uart2_Busy());                    // Remove when better system is implemented.
        P2OUT ^= GPIO_PIN2;
        logicChar = nextChar;
        return 1;
    }

    return 0;
}
