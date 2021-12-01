/*
 * fungen.h
 *
 * Description: Header file containing necessary definitions for function generator operation.
 *
 */

#ifndef FUNGEN_H_
#define FUNGEN_H_

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

// Macros
#define SLAVE_ADDR  (0x4C) // I2C Address of the DAC
#define CH_OFF      (0X00) // Function Generator turned off.
#define CH_A        (0x10) // 0x10 = Output to Channel A
#define CH_B        (0x12) // 0x12 = Output to Channel B
#define CH_C        (0x14) // 0x14 = Output to Channel C
#define CH_D        (0x16) // 0x14 = Output to Channel D


void Init_FunGen(void);
uint8_t FunGen_Process();
void Update_Signal_Array(void);
void Set_Active_Channel(uint8_t value);
void Set_Active_Type(uint16_t value);
void Set_Active_Bias(float value);
void Set_Active_Amp(float value);

#endif /* FUNGEN_H_ */
