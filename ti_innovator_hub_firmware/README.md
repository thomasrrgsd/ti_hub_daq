
# TI Innovator HUB Firmware (MSP432P401R)


## Table of Contents

* [Introduction](#introduction)
* [Software](#software)
* [Hardware](#hardware)
* [Bugs and To Do](#bugs-and-to-do)

## Introduction


The TI Innovator HUB is the center of this project. Each module connects to the HUB through its many grove ports described in the [Hardware](#hardware) section. When the HUB is powered on, it intializes all its peripherals and configures the multimeter. Then it runs in a loop where it waits for incoming serial data from the wifi module. When a command is given, the HUB responds. When a module is turned on (e.g. Oscilloscope), then the data is recorded and turned into a data packet. This data packet is updated and sent out to the wifi module repeatedly in the loop. The HUB is always communicating with the wifi module, but if nobody is connected to the wifi, then the data is just dumped in the wifi module.

## Software

The firmware for the TI Innovator HUB was written with Code Composer Studio (CCS) 10.4. This project uses software abstraction layers from the driverlib package. The project requires [driverlib](https://www.ti.com/tool/MSPDRIVERLIB) to run. Addtionally, the TI Innovator HUB contains an MSP432P401R and so in order to flash the HUB, you must have the MSP432 drivers installed.
It is intended that the CCS project files will be converted into a TI Innovator HUB sketch in the future to allow flashing with the sketch using the appropriate tool. The firmware utilizes many of the HUBs internal peripherals to perform its functions. The UART, I2C, DMA, ADC, TIMER_A, and Interrupt vectors are used. The theory of operation is not covered here, but is instead found in the [project_files folder](https://github.com/thomasrrgsd/ti_hub_daq/tree/main/project_files).

## Hardware

The TI Innovator HUB has an MSP432P401R inside of it and this was used to prototype the TI Innovator HUB DAQ. The TI Innovator HUB documentation specifies how the port pins are mapped to the external connectors. The pin mappings are given below in addition with there function with regard to the project:
- IN1 / P5.4 and P5.5 / Not used
- IN2 / P4.5 and P4.7 / A8 and A6 (Oscilloscope ch 2 and 1 respectively)
- IN3 / P4.4 / Not used
- OUT1 / P3.2 and P3.3 / Wifi module UART RX and TX
- OUT2 / P3.5 and P3.7 / Not used
- OUT3 / Power input for wifi module
- I2C / P6.4 and P6.5 / Multimeter and Function Generator
- Breadboard / {P2.6, P2.4, P2.7, P5.1, P5.2, P5.0, P3.0, P1.5, P1.6, P1.7} / Logic Analyzer channels 1-8.

Additionally, you can look at the firmware to see which pins are mapped to which device.

## Bugs and To Do

### To Do
- Fix magic numbers: There are numbers in the code that represent something but are not obvious do to not using obvious names.
- Update variable types to uint8_t and uint16_t, etc: For uniformity and also to make sure that code is compatible between different devices if needed. Only important in the case where a variable, such as int is a different size on different devices.
- Increase robustness of firmware: The firmware doesn't operate as efficient as it could. Some areas of code rely on time delays. There are bugs that can hang up the system and requires a restart. Some issues can be solved by implementing more error checking to increase robustness.
- Increase modularity and minimize use of global variables: We want each peripheral and module to have its own .c and .h file. This provides a nice API that abstracts and simplifies coding as the project becomes more complex (it got very complex). Additionally, not protecting code and using bad practices, such as global variables can easily cause or be the cause of bugs due to uncoordinated access to a global variable.
- Switch firmware to RTOS: Right now we have to use delays to make the modules operate together. A robust system for coordinating operation is needed. This can be best provided by an RTOS.
- Change I2C to DMA: Interrupts introduce tremendous overhead. This is a significant overhead with regards to the function generator.

### Bugs

- Without a delay after initialization in main, the firmware does not operate correctly when not in debug mode. Most likely due to the I2C not being ready before initializing the multimeter directly after enabling interrupts.
- When receiving serial data (command packet) on UART2 RX, sometimes the data will be dropped and hangup the serial data stream. This began happening when the baud rate was increased to 4Mbps. We knew that raising the baud this high would introduce errors and there is no existing system to account for this error. This is one major reason for using time delays to send information. Ideally, the GUI or ESP32 would notify the MSP432 when it can send its next data, but every so often it hangs up without error checking. On a period delay, if data is missed, then it just keeps going.
- Multimeter and Function Generator must be plugged into I2C for the project to work. This is due to using a while loop with the multimeter and function generator I2C code that looks for a successful transmit. If the transmit does not succeed, then the program hangs up. There are better ways to implement this system and we just did not have time to get around to it.
