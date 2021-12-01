
/*------------------------------------------------------------------------------
 *
 * William T. Jarratt
 * 18 MAR 2021
 * Built with Code Composer Studio: 10.2.0.000009
 *
 * Description: This file contains definitions for the project. Definitions
 * in macros.h are used throughout the whole project. Definitions included
 * from other header files are used only for the associated .c file.
 *
 * Included Headers:
 *      - ex: ports.h, definitions unique to ports.c
 *      -
 *      -
 *
 *----------------------------------------------------------------------------*/

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "functions.h"
#include "multimeter.h"

#ifndef MACROS_H_
#define MACROS_H_

#define MCLK_FREQ           (12000000)
#define SMCLK_FREQ          (12000000)

/*----------------------------------------------------------------------------
 * General Definitions
 *----------------------------------------------------------------------------*/
#define TRUE                (1)     // Indicates true
#define FALSE               (0)     // Indicates false
#define INIT_ZERO           (0)     // Initial value is zero
#define RESET_ZERO          (0)     // Reset value to zero
#define SUCCESS             (0)
#define FAILURE             (1)
//#define NULL                (0)

/*----------------------------------------------------------------------------
 * Main Process Definitions
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * UART Definitions
 *----------------------------------------------------------------------------*/
#define NO_INTR             (0x00)  // No interrupt happened
#define RX_INTR             (0x02)  // RX buffer full
#define TX_INTR             (0x04)  // TX buffer empty

#define BUFFER_UART         (150)    // 150 Byte buffer

/*----------------------------------------------------------------------------
 * I2C Definitions
 *----------------------------------------------------------------------------*/
#define FG_ADDR             (0x00)  // Address of the Function Generator
#define I2C_BUFFER_SIZE     (8)     // Size of I2C buffer for writing

/*----------------------------------------------------------------------------
 * Timer Definitions
 *----------------------------------------------------------------------------*/
#define STCSR_EN            (0x01)  // Enable SysTick timer
#define STCSR_INT           (0x02)  // Enable SysTick interrupt
#define STCSR_MCLK          (0x04)  // Set SysTick source to MCLK
#define STCSR_FLAG          (0x10000) // SysTick counter flag

#define CCR1_INTR           (0x02)    // CCR1 IV value
#define CCR2_INTR           (0x04)    // CCR2 IV value

#endif /* MACROS_H_ */
