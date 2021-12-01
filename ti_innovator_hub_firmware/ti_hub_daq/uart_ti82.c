/*
 * uart.c
 *
 * Description: The UART allows for debugging with a serial terminal and also
 * to communicate with the wifi module. The UART was initially setup to communicate
 * using interrupts. However, the current method of operation is DMA. The interrupts
 * were left in as a way to migrate back to interrupts if needed. It is possible that
 * UART0 will need to remain using interrupts due to needing that DMA channel for I2C.
 * Since the I2C is not configured for DMA UART0 is currently using DMA. The data
 * received over UART is processed within this .c file.
 *
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <string.h>
#include "dma_ti82.h"
#include "macros.h"
#include "ports.h"
#include "uart_ti82.h"

uint8_t command_flag[100];
uint32_t command_value[100];

volatile char rx2_ring_buffer[BUFFER_UART];  // Ring buffer received from Wifi
volatile int  rx2_ring_write;                // Index for writing to ring buffer
         int  rx2_ring_read;                 // Index for reading from ring buffer

volatile char rx1_ring_buffer[BUFFER_UART];  // Ring buffer received from Wifi
volatile int  rx1_ring_write;                // Index for writing to ring buffer
         int  rx1_ring_read;                 // Index for reading from ring buffer

// Essentially a rx2_process
         char data_buffer[BUFFER_UART];           // Ring buffer received from Wifi
         int  data_index = 0;                     // Index for sending out TX buffer

                // Not implemented.
         char rx1_process[BUFFER_UART];           // Ring buffer received from Wifi
         int  rx1_index = 0;                     // Index for sending out TX buffer

volatile char tx2_process[BUFFER_UART];  // Buffer for transmitting to wifi
         int  tx2_index;                     // Index for sending out TX buffer
volatile char tx2_done;

volatile char tx1_process[BUFFER_UART];  // Buffer for transmitting to wifi
         int  tx1_index;                     // Index for sending out TX buffer
volatile char tx1_done;


/*
 * Init_UART0
 *
 * UART Channel 0 communicates with the PC. Used for debug.
 *
 */
void Init_UART0(void){

    int i; // Index variable

    // NULL out the ring buffer and process buffer and reset the indexes
    rx1_ring_write  = INIT_ZERO;
    rx1_ring_read   = INIT_ZERO;
    rx1_index       = INIT_ZERO;
    tx1_index       = INIT_ZERO;
    tx1_done        = TRUE;

    for(i = RESET_ZERO; i < BUFFER_UART; i++){

        rx1_ring_buffer[i]  = '\0';
        rx1_process[i]      = '\0';
        tx1_process[i]      = '\0';
    }

    // If UART0 is initialized to 460800 baud rate.
    // N = 12MHz / 460800 = 26.041667
    // OS16 = 1, UCBRx = INT(N/16) = INT(1.627604) = 1,
    // UCBRFx = INT([(N/16) - INT(N/16)]*16) = INT([0.627604]*16) = INT(10.04167) = 10
    // A fraction of 0.04167 gives BRSx = 0x0
    EUSCI_A0->CTLW0 |=  1;
    EUSCI_A0->MCTLW  =  0;
    EUSCI_A0->CTLW0  =  0x0081;
    EUSCI_A0->BRW    =  1;
    EUSCI_A0->MCTLW  =  0x00A1; // Set for 460800 baud at 12MHz
    EUSCI_A0->CTLW0 &= ~1;
    EUSCI_A0->IE    |=  UCRXIE; // Enable RX interrupts.
}

/*
 *  Init_UART2
 *
 *  UART Channel 2 communicates with the Wifi Module.
 */
void Init_UART2(void){

    int i; // Index variable

    // NULL out the ring buffer and process buffer and reset the indexes
    rx2_ring_write  = INIT_ZERO;
    rx2_ring_read   = INIT_ZERO;
    tx2_index       = INIT_ZERO;
    tx2_done        = TRUE;
    data_index      = INIT_ZERO;

    for(i = RESET_ZERO; i < sizeof(rx2_ring_buffer); i++){

        rx2_ring_buffer[i]  = '\0';
        tx2_process[i]      = '\0';
        data_buffer[i]      = '\0';
    }

    EUSCI_A2->CTLW0 |=  1;
    EUSCI_A2->MCTLW  =  0;
    EUSCI_A2->CTLW0  =  0x0081;
    EUSCI_A2->BRW    =  3;
    EUSCI_A2->MCTLW  =  0x0000;     // Set for 4M baud at 12MHz
    EUSCI_A2->CTLW0 &= ~1;
    //EUSCI_A2->IE    |=  UCRXIE; // Enable RX interrupts. Not needed when using DMA.
}


// Transmits string to terminal for debugging.
void TX_Debug(const char* tx_string){

    int i;          // Index variable
    int tx_size;    // Size of string to be TXed

    // Determine the string size of tx_string, sizeof() does not work
    for(i = RESET_ZERO; i < sizeof(tx1_process); i++){ // tx_string size
        if(tx_string[i] == '\0'){
            tx_size = i;
            break;
        }
    }

    // Clear out tx process buffer and then copy tx_string to the tx process buffer
    for(i = RESET_ZERO; i < sizeof(tx1_process); i++){
        tx1_process[i] = '\0';
    }

    for(i = RESET_ZERO; i <= tx_size; i++){
        tx1_process[i] = tx_string[i];
    }

    // String has been copied over, enable TX interrupt
    // This kicks off the transmit
    tx1_done = FALSE;
    EUSCI_A0->IE |=  UCTXIE;
}


void Clear_Data(void){
    int i = 0;
    data_index = 0;
    for(i = 0; i < sizeof(data_buffer); i++){
        data_buffer[i] = '\0';
    }
}

/*
 *  Checks for Commands from GUI via the UART RX2.
 *
 *  First checks to see if a command is ready on RX2 via DMA5.
 *  If so, then cmd_ptr contains the command packet. Byte 0 is
 *  a sentinel character to be used for error checking. Byte 1
 *  is the command indicator from 1 to 100. Bytes 2 to 5 are
 *  a 32 bit value with byte 2 being the MSB and byte 5 being
 *  the LSB. We set the appropriate flag and copy over the
 *  appropriate 4 bytes of data.
 *
 */
void RX2_Command(void){

    uint8_t cmd_ptr[6];
    uint32_t temp_val = 0;
    uint8_t i = 0;

    if(Dma_Ch5_RX2_Ready(cmd_ptr)){ // No commands are ready
        return;
    }

    // Command is ready, process the received command.
    // First check sentinel character. Will add later. This is for error checking.

    // Then set command flag.
    command_flag[cmd_ptr[1]] = TRUE;

    // Fill in command value with elements 2,3,4,5. MSB to LSB.

    for(i = 2; i < 6; i++){
        temp_val = temp_val << 8;
        temp_val |= cmd_ptr[i];
    }
    command_value[cmd_ptr[1]] = temp_val;
    //P2OUT ^= GPIO_PIN1;         // Toggle LED for debugging.
}

/*
 * Checks the status of command flag number 'comm'.
 *
 */
uint8_t Check_Command(uint8_t comm){
    return command_flag[comm];
}

/*
 * Sets command number 'comm' flag to FALSE (0).
 *
 */
void Clear_Command(uint8_t comm){
    command_flag[comm] = FALSE;
}

/*
 * Returns the 32 bit value for command number 'comm'.
 *
 */
uint32_t Get_Command_Value(uint8_t comm){
    return command_value[comm];
}

/*
 *
 *
 */
void Set_Command_Value(uint8_t comm, uint32_t value){
    command_value[comm] = value;
}

// RX0 Update is used to receive commands from a serial terminal for debugging.
void RX0_Update(void){

    int temp_index = 0;

    if(rx1_ring_read != rx1_ring_write){ // Need to update read index,
        temp_index = rx1_ring_read++;
        if(rx1_ring_buffer[temp_index]){
            rx1_process[rx1_index++] =  rx1_ring_buffer[temp_index];
        }
        if(rx1_index > BUFFER_UART){
            rx1_index = 0;
        }
    }

    if(rx1_ring_read >= BUFFER_UART){ // Ring around the rosy
        rx1_ring_read = RESET_ZERO;
    }
}


void RX0_Clear(void){
    int i = 0;
    rx1_index = 0;
    for(i = 0; i < BUFFER_UART; i++){
        rx1_process[i] = '\0';
    }
}


void RX0_Flag(void){

    if(strstr(rx1_process, "high\r\n")){
        P2->OUT  |=  BLUE_LED;
        RX0_Clear();
    }

    if(strstr(rx1_process, "low\r\n")){
        P2->OUT  &= ~BLUE_LED;
        RX0_Clear();
    }

}

// UART0 interrupt handler.
// UART0 communicates with the PC.
// Interrupt is not needed when DMA is being used.
void EUSCIA0_IRQHandler(void){

    char temp;

    switch(EUSCI_A0->IV){
    case NO_INTR:

        break;

    // -------------------------------------------------
    case RX_INTR:
        temp = EUSCI_A0->RXBUF;
        rx1_ring_buffer[rx1_ring_write++] = temp; // Place char in ring
        EUSCI_A0->TXBUF = temp; // Echo character to PC for debugging

        // Wrap the ring around the rosy
        if(rx1_ring_write >= BUFFER_UART){
            rx1_ring_write = RESET_ZERO;
        }
        break;

    // -------------------------------------------------
    case TX_INTR:
        EUSCI_A0->TXBUF = tx1_process[tx1_index++];   // Send out character pointed by index

        if(tx1_process[tx1_index] == '\0'){ // Reached end of string
            tx1_done = TRUE;
            EUSCI_A0->IE &= ~UCTXIE;        // Disable TX interrupt and reset index
            tx1_index     =  RESET_ZERO;
        }
        else if(tx1_index >= BUFFER_UART){  // Reached the end of buffer without NULL

            EUSCI_A0->IE &= ~UCTXIE;        // Disable TX interrupt and reset index
            tx1_index     =  RESET_ZERO;
        }
        break;

    default: break;
    }
}

// UART2 interrupt handler.
// UART2 communicates with the Wifi Module.
// Interrupt is not needed when DMA is being used.
void EUSCIA2_IRQHandler(void){

    char temp;

    switch(EUSCI_A2->IV){ // Checks interrupt vector to see if TX or RX caused flag.
    case NO_INTR:
        break;

    // -------------------------------------------------
    case RX_INTR:   // RX interrupt handle data
        temp = EUSCI_A2->RXBUF;
        rx2_ring_buffer[rx2_ring_write++] = temp; // Place char in ring
        //EUSCI_A0->TXBUF = temp; // Echo character to PC for debugging

        // Wrap the ring around the rosy
        if(rx2_ring_write >= sizeof(rx2_ring_buffer)){

            rx2_ring_write = RESET_ZERO;
        }
        break;

    // -------------------------------------------------
    case TX_INTR:   // TX interrupt, send out data
        EUSCI_A2->TXBUF = tx2_process[tx2_index++];   // Send out character pointed by index

        if(tx2_process[tx2_index] == '\0'){ // Reached end of string
            tx2_done = TRUE;
            EUSCI_A2->IE &= ~UCTXIE;        // Disable TX interrupt and reset index
            tx2_index     =  RESET_ZERO;
        }
        else if(tx2_index >= sizeof(tx2_process)){  // Reached the end of buffer without NULL

            EUSCI_A2->IE &= ~UCTXIE;        // Disable TX interrupt and reset index
            tx2_index     =  RESET_ZERO;
        }
        break;

    default: break; // Other interrupt source
    }
}
