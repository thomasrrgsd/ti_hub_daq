/*
 * uart.h
 *
 * Description: Header file containing definitions for UART operation.
 *
 */

#ifndef UART_TI82_H_
#define UART_TI82_H_

// Macros
#define BUFFER_UART2RX      (1024)

#define NO_INTR             (0x00)  // No interrupt happened
#define RX_INTR             (0x02)  // RX buffer full
#define TX_INTR             (0x04)  // TX buffer empty

#define BUFFER_UART         (150)    // 150 Byte buffer

// External Global Variables

// Function Declarations
void Init_UART0(void);
void Init_UART2(void);
void TX_Debug(const char* tx_string);
void Clear_Data(void);
void RX2_Command(void);
uint8_t Check_Command(uint8_t comm);
void Clear_Command(uint8_t comm);
uint32_t Get_Command_Value(uint8_t comm);
void Set_Command_Value(uint8_t comm, uint32_t value);
void RX0_Update(void);
void RX0_Clear(void);
void RX0_Flag(void);

#endif /* UART_TI82_H_ */
