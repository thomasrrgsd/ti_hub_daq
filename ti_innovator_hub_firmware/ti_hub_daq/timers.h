/*
 * timers.h
 *
 * Description: Header file containing definitions for timers Ax and Systick timer.
 *
 */

#ifndef TIMERS_H_
#define TIMERS_H_

// Macros
#define STCSR_EN            (0x01)  // Enable SysTick timer
#define STCSR_INT           (0x02)  // Enable SysTick interrupt
#define STCSR_MCLK          (0x04)  // Set SysTick source to MCLK
#define STCSR_FLAG          (0x10000) // SysTick counter flag

#define CCR1_INTR           (0x02)    // CCR1 IV value
#define CCR2_INTR           (0x04)    // CCR2 IV value

// Function definitions
void Init_SysTimer(void);
void Init_TimerA0(void);
void Init_TimerA1(void);
void msDelay(int delay_val);
void Set_Timer_Frequency(uint32_t frequency);

// External global variables


#endif /* TIMERS_H_ */
