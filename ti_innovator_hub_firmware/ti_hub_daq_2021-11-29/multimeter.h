/*
 * multimeter.h
 *
 *  Created on: Sep 26, 2021
 *
 */

#ifndef MULTIMETER_H_
#define MULTIMETER_H_

// Macros
#define DMM_ADDR            (0x40)  // Address of Multi-meter

#define DMM_CAL_VAL         (0x13B2)// INA226 Calibration value
#define DMM_CONF_VAL        (0x4327)// INA226 Configuration Value

#define CONF_REG            (0x00)  // Configuration Register
#define SHUNT_REG           (0x01)  // Shunt Voltage Register
#define BUS_REG             (0x02)  // Bus Voltage Register
#define POWER_REG           (0x03)  // Power Register
#define CURRENT_REG         (0x04)  // Current Register
#define CAL_REG             (0x05)  // Calibration Register

#define SHUNTV              ('d')   // Shunt Voltage
#define BUSV                ('b')   // Bus Voltage
#define POWER               ('p')   // Power
#define CURRENT             ('c')   // Current

// Function declarations
int Calibrate_Multimeter(unsigned int calibrate_value);
int Configure_Multimeter(unsigned int configure_value);
int Read_Multimeter_Value(unsigned char this_register);
void Json_Dmm_Str(void);
void Multimeter_Process(void);

// Relevant global variables
extern int  bus_voltage;
extern int  dmm_values[5];
extern char dmm_value_select;

#endif /* MULTIMETER_H_ */
