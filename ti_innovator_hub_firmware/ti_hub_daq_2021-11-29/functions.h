/*
 * functions.h
 *
 *  Created on: Apr 14, 2021
 *      Author: William Jarratt
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

// Interrupts
void Init_Interrupts(void);

// Timers
void Init_SysTimer(void);
void Init_TimerA0(void);
void Init_TimerA1(void);
void msDelay(int delay_val);
void Set_Timer_Frequency(uint32_t frequency);

// String operations
void clearSTR(char* str);
void reverse(char* str, int len);
int intToStr(int x, char str[], int d);
void ftoa(float n, char* res, int afterpoint);
void HexToStr(int hexVal);

// I2C Serial Communication
void Init_I2C(void);
int I2C1_Write(int sec_addr, unsigned char mem_addr, unsigned char data1, unsigned char data2);
int I2C1_Read(int sec_addr, unsigned char mem_addr, unsigned char* data1, unsigned char* data2);
int I2C1_TX_RX(unsigned char slave_addr, unsigned char* data, int data_len, char rw);
char Is_I2C_Ready();
int Get_I2C_Data();

#endif /* FUNCTIONS_H_ */
