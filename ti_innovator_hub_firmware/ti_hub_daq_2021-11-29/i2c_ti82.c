/*
 * i2c.c
 *
 *  Created on: Apr 6, 2021
 *      Author: William Jarratt
 */


#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "i2c_ti82.h"
#include "macros.h"
#include "ports.h"

unsigned char   i2c_tx[I2C_BUFFER_SIZE];
int             i2c_tx_index = 0;
int             i2c_tx_len = 0;
volatile char   i2c_tx_ready = TRUE;
//char            i2c_tx_ready = TRUE;

char            i2c_read = FALSE;
char            i2c_read_count = 0;

int             i2c_data = 0;


void Init_I2C(void){

    int i = 0;
    for(i = 0; i < I2C_BUFFER_SIZE; i++){
        i2c_tx[i] = 0;
    }

    EUSCI_B1->CTLW0 |= 0x01;        // I2C held in reset state to allow configuration.

    EUSCI_B1->CTLW0 = 0x0F81;       // SMCLK source, Synchronous mode EN, I2C selected, Master device, 7bit Slave, 7bit master

    // Per I2C specification: tlow,min = thigh,min = 5 uSec
    // EUSCI_B1->BRW = 30;      // 3MHZ SMCLK
    // EUSCI_B1->BRW = 480;     // 48MHZ SMCLK
    //EUSCI_B1->BRW = 120;        // 12MHZ SMCLK, 100Kbps
    EUSCI_B1->BRW = 30;        // 12MHZ SMCLK, 400Kbps

    P6->SEL0 |=  UCB1SDA;       // Port 6 Pin 4 is I2C Signal Data
    P6->SEL1 &= ~UCB1SDA;


    P6->SEL0 |=  UCB1SCL;       // Port 6 Pin 5 is I2C Signal Clock
    P6->SEL1 &= ~UCB1SCL;

    EUSCI_B1->CTLW0 &= ~1;      // Reset is released. I2C is not operational.

    // Interrupts are not enabled automatically, enabled when needed.
}

/*
 * I2C read and write using interrupts. You tell this function what data to send,
 * how much data, and to what address. It will copy the data and start the interrupts.
 * The interrupts will handle data transmission. i2c_tx_ready is set false when a
 * data transfer is started. If this function is called while another transfer
 * is in progress, nothing will happen and a 1 is returned. Return 0 when transfer
 * successfully started.
 *
 */
int I2C1_TX_RX(unsigned char slave_addr, unsigned char* data, int data_len, char rw){

    if(!i2c_tx_ready){
        return 1;
    }
    i2c_tx_ready = FALSE;
    i2c_tx_index = 0;

    i2c_read = rw;
    i2c_read_count = 0;

    EUSCI_B1->IE &= ~0x02;          // Disable transmit interrupt.
    EUSCI_B1->IE &= ~0x01;          // Disable receive interrupt.
    EUSCI_B1->IE    &=  ~0x08;
    EUSCI_B1->IE    &=  ~0x20;

    int i = 0;
    i2c_tx_len = data_len;
    for(i = 0; i < i2c_tx_len; i++){
        i2c_tx[i] = data[i];
    }

    EUSCI_B1->I2CSA  =  slave_addr;  // Set the slave address.
    EUSCI_B1->IFG   &= ~0x20;        // Clear a pending NACK interrupt flag.
    EUSCI_B1->IE    |=  0x20;        // Enable NACK interrupt.
    EUSCI_B1->IFG   &= ~0x08;        // Clear a pending stop interrupt flag.
    EUSCI_B1->IE    |=  0x08;        // Enable stop interrupt.
    EUSCI_B1->IFG   &= ~0x02;        // Clear a pending transmit interrupt flag.
    EUSCI_B1->IE    |=  0x02;        // Enable transmit interrupt.
    EUSCI_B1->CTLW0 |=  0x0010;      // Enable master transmitter mode.
    EUSCI_B1->CTLW0 |=  0x0002;      // Generate START and send slave address.


    return 0;
}

char Is_I2C_Ready(){
    return i2c_tx_ready;
}

int Get_I2C_Data(){
    return i2c_data;
}

/*
 * I2C interrupt handler.
 *
 */
void EUSCIB1_IRQHandler(void){

    switch(EUSCI_B1->IV){
    case 0x00: // No interrupt
        break;
    case 0x04: // Not acknowledgment; UCNACKIFG
        //P2->OUT ^= GPIO_PIN0;
        EUSCI_B1->IE    &= ~0x23;               // Disable NACK, RX, and TX interrupts
        EUSCI_B1->CTLW0 |=  0x04;               // Send stop
        break;
    case 0x06: // Start condition; UCSTTIFG
        break;
    case 0x08: // Stop condition; UCSTPIFG
        i2c_tx_ready = TRUE;                    // Ready for next I2C operation
        dmm_values[dmm_value_select] = i2c_data;
        EUSCI_B1->IE &= ~0x08;                  // Disable stop interrupt
        //P2->OUT ^= GPIO_PIN0;                 // Debugging
        break;
    case 0x16: // Data received; UCRXIFG0
        if(i2c_read_count == 0){                // Expect 2 bytes: MSB, LSB
            i2c_data = EUSCI_B1->RXBUF;
            i2c_data = i2c_data << 8;           // MSB is first read
            i2c_read_count++;
            EUSCI_B1->CTLW0 |= 0x04;            // Transmit stop for next bit
        }                                       // Next bit will be last bit
        else{
            i2c_data |= EUSCI_B1->RXBUF;        // Second byte is LSB
            EUSCI_B1->IE &= ~0x01;              // After 2 bytes, disable RX interrupt
        }
        break;
    case 0x18: // Transmit ready; UCTXIFG0
        //TX_Debug("Transmit\r\n");
        if(i2c_tx_index >= i2c_tx_len){         // Done transmitting
            EUSCI_B1->IE    &= ~0x02;           // Disable transmit interrupt
            if(i2c_read){   // Expect a read.
                EUSCI_B1->IFG &= ~0x01;         // Clear pending receive flag
                EUSCI_B1->IE |=  0x01;          // Enable receive interrupt
                EUSCI_B1->CTLW0 &= ~0x0010;     // Set to receiver mode
                EUSCI_B1->CTLW0 |=  0x0002;     // Generate restart with read bit
            }
            else{ EUSCI_B1->CTLW0 |=  0x04; }   // Send stop, no read expected
            break;
        }
        EUSCI_B1->TXBUF = i2c_tx[i2c_tx_index]; // Send out data.
        i2c_tx[i2c_tx_index++] = 0;             // Clear TX buffer as each byte is sent
        break;
    default: break;
    }
}

