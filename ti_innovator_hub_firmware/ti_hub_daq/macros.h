
/*------------------------------------------------------------------------------
 *
 * macros.h
 *
 * Description: This file contains definitions for the project. Definitions
 * in macros.h are used throughout the whole project. This header file would
 * ideally only contain definitions that do not fit into other headers.
 *
 *----------------------------------------------------------------------------*/

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

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
//#define NULL                (0)   // Collision with other header file.

#endif /* MACROS_H_ */
