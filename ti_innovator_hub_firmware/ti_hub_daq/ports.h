/*
 * ports.h
 *
 * Description: Port pin header definitions.
 *
 */

#ifndef PORTS_H_
#define PORTS_H_

// Function Declarations
void Init_Ports(void);
void Init_Port_1(void);
void Init_Port_2(void);
void Init_Port_3(void);
void Init_Port_4(void);
void Init_Port_5(void);
void Init_Port_6(void);
void Init_Port_7(void);

// Port Definitions
// Port 1 Pin Definitions
#define LED1        (0x01)  // Pin 0
#define SW1         (0x02)  // Pin 1
#define UCA0RXD     (0x04)  // Pin 2
#define UCA0TXD     (0x08)  // Pin 3
#define SW2         (0x10)  // Pin 4
#define P1_5        (0x20)  // Pin 5
#define P1_6        (0x40)  // Pin 6
#define P1_7        (0x80)  // Pin 7

// Port 2 Pin Definitions
#define P2_0        (0x01)  // Pin 0
#define P2_1        (0x02)  // Pin 1
#define BLUE_LED    (0x04)  // Pin 2
#define P2_3        (0x08)  // Pin 3
#define P2_4        (0x10)  // Pin 4
#define P2_5        (0x20)  // Pin 5
#define P2_6        (0x40)  // Pin 6
#define P2_7        (0x80)  // Pin 7

// Port 3 Pin Definitions
#define P3_0        (0x01)  // Pin 0
#define P3_1        (0x02)  // Pin 1
#define UCA2RXD     (0x04)  // Pin 2
#define UCA2TXD     (0x08)  // Pin 3
#define P3_4        (0x10)  // Pin 4
#define P3_5        (0x20)  // Pin 5
#define P3_6        (0x40)  // Pin 6
#define P3_7        (0x80)  // Pin 7

// Port 4 Pin definitions
#define P4_0        (0x01)  // Pin 0
#define P4_1        (0x02)  // Pin 1
#define P4_2        (0x04)  // Pin 2
#define P4_3        (0x08)  // Pin 3
#define P4_4        (0x10)  // Pin 4
#define V_CH_A8     (0x20)  // Pin 5
#define P4_6        (0x40)  // Pin 6
#define V_CH_A6     (0x80)  // Pin 7

// Port 5 Pin Definitions
#define P5_0        (0x01)  // Pin 0
#define P5_1        (0x02)  // Pin 1
#define P5_2        (0x04)  // Pin 2
#define P5_3        (0x08)  // Pin 3
#define V_CH_A1     (0x10)  // Pin 4
#define V_CH_A0     (0x20)  // Pin 5
#define P5_6        (0x40)  // Pin 6
#define P5_7        (0x80)  // Pin 7

// Port 6 Pin Definitions
#define P6_0        (0x01)  // Pin 0
#define P6_1        (0x02)  // Pin 1
#define P6_2        (0x04)  // Pin 2
#define P6_3        (0x08)  // Pin 3
#define UCB1SDA     (0x10)  // Pin 4
#define UCB1SCL     (0x20)  // Pin 5
#define P6_6        (0x40)  // Pin 6
#define P6_7        (0x80)  // Pin 7

#endif /* PORTS_H_ */
