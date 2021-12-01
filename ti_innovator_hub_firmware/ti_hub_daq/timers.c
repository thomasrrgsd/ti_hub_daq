/*
 * timers.c
 *
 * Description: All functions dealing with timing operations. SysTimer, Timer A
 * and their respective interrupts.
 *
 */

#include "macros.h"
#include "fungen.h"
#include "timers.h"

volatile char A0_CCR2_flag = FALSE; // Flag used for delay.

/*
 * May be implemented later as a heart beat timer. This was important when
 * the WiFi was being hosted on the MSP432 and there were a ton of while loops.
 */
void Init_SysTimer(void){

    // Configure SysTick
    SysTick->CTRL    = 0;           // Clear SysTick timer configuration register
    SysTick->CTRL   &= ~STCSR_INT;  // Disable interrupts
    SysTick->CTRL   |= STCSR_MCLK;  // Set clock source to MCLK

    // 8 MHZ
    SysTick->LOAD    = 8000000;     // Starting count value
    SysTick->VAL     = 0;           // Clear current count value

    SysTick->CTRL   |= STCSR_EN;    // Enable SysTick timer
}

/*
 * Initialize Timer A0
 *
 * - Timer A0 is ran in continuous mode. The CCR is configured as the current
 *   counter value + the offset of time you would like.
 */
void Init_TimerA0(void){

    TIMER_A0->CTL = 0x0264;         // SMCLK source - Divide by 2 - Overflow disabled
    TIMER_A0->EX0 = 7;              // Divide CLK source by 8 again

    TIMER_A0->CCR[0] = 5859;       // Set compare value for channel 0, default is 1 second periods at 5859 counts.
    TIMER_A0->CCTL[0] |=  0x10;      // Enable compare value interrupt for channel 0
    //TIMER_A0->CCTL[0] &= ~0x10;      // Disable compare value interrupt for channel 0

    TIMER_A0->CCR[1] = 23439;
    //TIMER_A0->CCTL[1] |= 0x10;

    TIMER_A0->CCR[2] = 46881;
    //TIMER_A0->CCTL[2] |= 0x10;
}

/*
 * Initialize Timer A1
 *
 * Timer A1 is ran in up mode and is used to trigger DMA channel 2 and 3.
 * This is done by using the count up mode where the timer resets to zero
 * upon reaching its CCR. CCR0 is used to trigger DMA channel 2 and CCR2 is
 * used to trigger DMA channel 3. The interrupt must be disabled in order for
 * the DMA transfer to be enabled.
 *
 */
void Init_TimerA1(void){

    TIMER_A1->CTL = 0x0254;             // SMCLK with divide by 2
    TIMER_A1->EX0 = 0;                  // Divide CLK source by 1 again

    TIMER_A1->CCR[0] = 300;       // Set compare value for channel 0
                                         // 300 = 50 usec
    //TIMER_A1->CCTL[0] |= 0x10;      // Enable compare value interrupt for channel 0

    //TIMER_A1->CCR[1] = 23439;
    //TIMER_A1->CCTL[1] |= 0x10;

    TIMER_A1->CCR[2] = 300;
    //TIMER_A1->CCTL[2] |= 0x10;
}

/*
 * Timer A_0 capture compare 0 interrupt
 */

// Temp global variable
uint16_t timer_freq = 5859;
void TA0_0_IRQHandler(void){


    TIMER_A0->CCTL[0] &= ~1;    // Clear interrupt flags
    TIMER_A0->CCR[0] = TIMER_A0->R + timer_freq;

    FunGen_Process();
}

void Set_Timer_Frequency(uint32_t frequency){
    timer_freq = (uint16_t)frequency;
}




/*
 * Timer A_0 all other interrupt sources. Use interrupt vector register (IV)
 * to determine which CCR caused the interrupt.
 *
 */
void TA0_N_IRQHandler(void){

    switch(TIMER_A0->IV){
    case CCR1_INTR:  // CCR1
        TIMER_A0->CCTL[1] &= ~1;
        TIMER_A0->CCR[1] += 23439;
        P2->OUT ^= GPIO_PIN0;
        break;
    case CCR2_INTR:  // CCR2
        TIMER_A0->CCTL[2] &= ~0x10; // Disable interrupt
        TIMER_A0->CCTL[2] &= ~1;    // Clear interrupt flag
        A0_CCR2_flag = TRUE;
        break;
    default: break;
    }
}

/*
 * Timer A_1 capture compare 0 interrupt
 */
void TA1_0_IRQHandler(void){


    TIMER_A1->CCTL[0] &= ~1;    // Clear interrupt flag.


    //TIMER_A1->CCR[0] = TIMER_A0->R + 96;

    //P2->OUT ^= GPIO_PIN2;
    //MAP_DMA_requestSoftwareTransfer(0);
}

// Example of a delay implemented using TIMER_A0 CCR2
// At 48MHz with 64 divider, limit delay to 85 mSec
void msDelay(int delay_val){

    A0_CCR2_flag = FALSE;

    TIMER_A0->CCTL[2] &= ~1;    // Clear interrupt flag
    TIMER_A0->CCR[2] = TIMER_A0->R + (750 * delay_val); // Set count amount for interrupt
    TIMER_A0->CCTL[2] |= 0x10;  // Enable interrupt

    // This works, but using a while loop to hold up the processor is not
    // good practice. It takes away from other measurements and allows
    // for bugs later on that hang up the processor.
    while(!A0_CCR2_flag){
        // Wait
    }
}
