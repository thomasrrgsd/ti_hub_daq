/*
 * dma_ch2.h
 *
 *  Created on: Oct 16, 2021
 *      Author: willi
 */

#ifndef DMA_TI82_H_
#define DMA_TI82_H_

// Macros
#define DMA_CH0_FLAG    (0x01)  // Check Channel 0 flag in interrupt 0 handler.
#define DMA_CH1_FLAG    (0x02)  // Check Channel 1 flag in interrupt 0 handler.
#define DMA_CH2_FLAG    (0x04)  // Check Channel 2 flag in interrupt 0 handler.
#define DMA_CH3_FLAG    (0x08)  // Check Channel 3 flag in interrupt 0 handler.
#define DMA_CH4_FLAG    (0x10)  // Check Channel 4 flag in interrupt 0 handler.
#define DMA_CH5_FLAG    (0x20)  // Check Channel 5 flag in interrupt 0 handler.

#define ADC_BUFFER_LEN  (1024)  // Length of buffer for ADC to DMA transfer.

// External Global Variables
extern uint8_t adc6_busy_1;

// Function Declarations
void Init_DMA(void);
void Init_DMA_CH0(void);
void Init_DMA_CH2(void);
void Init_DMA_CH3(void);
void Init_DMA_CH4(void);
void Init_DMA_CH5(void);
uint8_t Dma_Ch0_Uart0_Busy(void);
uint8_t Dma_Ch0_Uart0_Send_String(uint8_t* str_ptr, uint16_t data_len);
uint8_t Dma_Ch2_ADC_Ch6_Get_Data(void);
uint8_t Dma_Ch3_ADC_Ch8_Get_Data(void);
uint8_t Dma_Ch4_Uart2_Busy(void);
uint8_t Dma_Ch4_Uart2_Send_String(uint8_t* str_ptr, uint16_t data_len);
uint8_t Dma_Ch5_RX2_Ready(uint8_t* dest_ptr);

#endif /* DMA_TI82_H_ */
