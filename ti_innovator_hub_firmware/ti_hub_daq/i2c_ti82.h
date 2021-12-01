/*
 * i2c_ti82.h
 *
 * Description: Header file containing definitions needed for I2C bus operation.
 */

#ifndef I2C_TI82_H_
#define I2C_TI82_H_

// Macros
#define BUFFER_UART2RX      (1024)

#define FG_ADDR             (0x00)  // Address of the Function Generator
#define I2C_BUFFER_SIZE     (8)     // Size of I2C buffer for writing

// External Global Variables

// Function Declarations
void Init_I2C(void);
int I2C1_TX_RX(unsigned char slave_addr, unsigned char* data, int data_len, char rw);
char Is_I2C_Ready();
int Get_I2C_Data();

#endif /* I2C_TI82_H_ */
