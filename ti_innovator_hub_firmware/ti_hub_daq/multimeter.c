/*
 * multimeter.c
 *
 * Description: INA226, used to measure voltage, current, and power. Can be setup to
 *              measure resistance.
 *
 *    Registers   00h Configuration   - R/W
 *                01h Shunt Voltage   - R       2.5 uV/bit
 *                02h Bus Voltage     - R       1.25 mV/bit
 *                03h Power           - R       25*(Current_LSB) W/bit
 *                04h Current         - R       Current_LSB A/bit
 *                05h Calibration     - R/W
 *
 * Note 1: The I2C that we are using is on pins 9(P6_5/SCL) and 10(P6_4/SDA).
 *
 * Note 2: INA226 booster address is h40. May be different, depending on A0 and A1 wiring.
 *         The address is h40 for the TI DAQ project, with A0 and A1 grounded.
 *
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "i2c_ti82.h"
#include "dma_ti82.h"
#include "macros.h"
#include "multimeter.h"
#include "timers.h"
#include "uart_ti82.h"

int     dmm_state = 0;
int     dmm_values[5] = {0, 0, 0, 0, 0};
char    dmm_value_select = 0; // 0 - Shunt, 1 - Bus, 2 - Power, 3 - Current, 4 - Unused
uint8_t dmm_blob[7] = {4, 0, 0, 0, 0, 0, 0};
uint16_t dmm_len = 7;

/*
 * This function writes to the calibration register. Returns 1 if unsuccessful,
 * returns 0 if calibration starts. Give it a 16b input. The calibration determines
 * how the measurements are scaled and calculated in the GUI.
 *
 */
int Calibrate_Multimeter(unsigned int calibrate_value){

    if(!Is_I2C_Ready()){    // I2C is not available, return 1
        return 1;
    }

    unsigned char bytes[3] = {CAL_REG, 0x00, 0x00}; // Set up I2C write data
    bytes[1] = (calibrate_value >> 8) & 0xFF;       // Send MSB then LSB
    bytes[2] = calibrate_value & 0xFF;

    I2C1_TX_RX(DMM_ADDR, bytes, 3, 0);              // Kick off I2C write

    return 0;
}

/*
 * Sets the DMM configuration register. Sets how measurements are taken
 * stored, and averaged.
 *
 */
int Configure_Multimeter(unsigned int configure_value){

    if(!Is_I2C_Ready()){    // I2C is not available, return 1
        return 1;
    }

    unsigned char bytes[3] = {CONF_REG, 0x00, 0x00}; // Set up I2C write data
    bytes[1] = (configure_value >> 8) & 0xFF;       // Send MSB then LSB
    bytes[2] = configure_value & 0xFF;

    I2C1_TX_RX(DMM_ADDR, bytes, 3, 0);              // Kick off I2C write

    return 0;
}

/*
 * This function reads value at the specified register. If I2C is busy or
 * if given an incorrect register value, returns 1. If I2C read is started,
 * returns 0. Give it one of these options: SHUNTV, BUSV, POWER, CURRENT.
 *
 */
int Read_Multimeter_Value(unsigned char this_register){

    if(!Is_I2C_Ready()){    // I2C is not available, return 1
        return 1;
    }

    unsigned char bytes[] = {0x00};

    switch(this_register){
    case SHUNTV:
        dmm_value_select = 0;
        bytes[0] = SHUNT_REG;
        I2C1_TX_RX(DMM_ADDR, bytes, 1, 1);
        return 0;
    case BUSV:
        dmm_value_select = 1;
        bytes[0] = BUS_REG;
        I2C1_TX_RX(DMM_ADDR, bytes, 1, 1);
        return 0;
    case POWER:
        dmm_value_select = 2;
        bytes[0] = POWER_REG;
        I2C1_TX_RX(DMM_ADDR, bytes, 1, 1);
        return 0;
    case CURRENT:
        dmm_value_select = 3;
        bytes[0] = CURRENT_REG;
        I2C1_TX_RX(DMM_ADDR, bytes, 1, 1);
        return 0;
    default:
        return 1;
    }
}

/*
 * Sequentially gets multimeter readings each time Multimeter_Process is called.
 * The I2C bus is being used by the function generator when this function is called.
 * The I2C bus must be taken from the function generator by disabling the function
 * generator and waiting for the last function generator write to finish.
 *
 */
void Multimeter_Process(void){

    TIMER_A0->CCTL[0] &= ~0x10;         // Disable compare value interrupt for channel 0.
                                        // This disables the operation of the function generator.
    int i = 0;
    for(i = 0; i < 10; i++){    // Give the function generator time to finish last update.
        msDelay(1);
    }

    // Code for DMM. Takes one measurement per call to Multimeter_Process.
    switch(dmm_state){
    case 0:
        Read_Multimeter_Value(BUSV);
        dmm_blob[2] = dmm_values[1] & 0xFF;
        dmm_blob[1] = (dmm_values[1] & 0xFF00) >> 8;
        dmm_state = 1;
        break;
    case 1:
        Read_Multimeter_Value(CURRENT);
        dmm_blob[4] = dmm_values[3] & 0xFF;
        dmm_blob[3] = (dmm_values[3] & 0xFF00) >> 8;
        dmm_state = 2;
        break;
    case 2:
        Read_Multimeter_Value(POWER);
        dmm_blob[6] = dmm_values[2] & 0xFF;
        dmm_blob[5] = (dmm_values[2] & 0xFF00) >> 8;
        dmm_state = 0;
        break;
    default:
        break;
    }

    for(i = 0; i < 10; i++){    // Allow multimeter to finish before giving I2C bus back to function generator.
        msDelay(1);
    }
    TIMER_A0->CCTL[0] |=  0x10;      // Enable compare value interrupt for channel 0

    Dma_Ch4_Uart2_Send_String(dmm_blob, dmm_len);   // Kickoff a serial data packet to wifi module.
}
