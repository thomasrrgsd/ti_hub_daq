/*
 * dma_ti82.c
 *
 *
 * Description: Sets up DMA channels and associated interrupts (0,1,2,3). The plan is to have
 * UART0, UART2, ADC14 Channel 1 and 2, and possibly the logic analyzer (8 port bits) utilize
 * the DMA. A stretch goal is to have I2C configured to use DMA.
 *
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "dma_ti82.h"
#include "macros.h"
#include "uart_ti82.h"

#pragma DATA_ALIGN(controlTable, 1024)  // Control data structure that is needed for
uint8_t controlTable[1024];             // DMA operation.

uint8_t dma_ch0_uart0_busy = FALSE;     // Is DMA0 currently sending data to UART0?
uint8_t dma_ch4_uart2_busy = FALSE;     // Is DMA4 currently sending data to UART2?

uint8_t command_buffer[25][6];          // Buffer to store commands as they are received.
uint8_t command_buffer_index = 0;       // Can store up to 25 unprocessed commands before
uint8_t command_buffer_read = 0;        // we start losing commands.

// ADC Channel 6 (P4.7) variables. ADC Ch. 6 measurements are 8 bits each.
uint8_t adc6_array_1[ADC_BUFFER_LEN];     // Element 1 is a sentinel character.
uint8_t adc6_busy_1 = FALSE;
uint8_t *adc6_active_buffer = 0;   //NULL; // Fix when string.h is dropped

// ADC Channel 8 (P4.5) variables. ADC Ch. 8 measurements are 8 bits each.
uint8_t adc8_array_1[ADC_BUFFER_LEN];     // Element 1 is a sentinel character.
uint8_t adc8_busy_1 = FALSE;
uint8_t *adc8_active_buffer = 0;   //NULL; // Fix when string.h is dropped


/*
 * Calls initialization for each DMA channel that is being used. Also enables
 * DMA module, sets up and configures control table, and enables interrupt 0.
 *
 */
void Init_DMA(void){

    MAP_DMA_enableModule();
    MAP_DMA_setControlBase(controlTable);

    Init_DMA_CH0();     // String to UART0 TX.
    Init_DMA_CH2();     // ADC Channel 6 to data array.
    Init_DMA_CH3();     // ADC Channel 6 to data array.
    Init_DMA_CH4();     // String to UART2 TX.
    Init_DMA_CH5();     // String from UART2 RX.

    MAP_Interrupt_enableInterrupt(DMA_INT0);
}

void Init_DMA_CH0(void){

    DMA_assignChannel(DMA_CH0_EUSCIA0TX);

    //Disabling channel attributes.
    MAP_DMA_disableChannelAttribute(DMA_CH0_EUSCIA0TX,
                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                    UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);

    //Setting Control Indexes.
    MAP_DMA_setChannelControl(UDMA_PRI_SELECT | DMA_CH0_EUSCIA0TX,
                              UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1);

    //Assigning/Enabling Interrupts.
    //Do not assign interrupt. Handled by interrupt 0 automatically.

    //Channel is enabled when a write to UART0 is ready.
    //MAP_DMA_enableChannel(0);
}

/*
 * Take 1023 samples of ADC14 Channel 6 and store in array.
 *
 */
void Init_DMA_CH2(void){

    // Initialize adc data array.
    int i = 0;
    for(i = 0; i < ADC_BUFFER_LEN; i++){
        adc6_array_1[i] = 0;
    }
    adc6_array_1[0] = 1; // Element 0 is the sentinel character. 1 indicates ADC channl 6(0).

    DMA_assignChannel(DMA_CH2_TIMERA1CCR0);

    //Disabling channel attributes.
    MAP_DMA_disableChannelAttribute(DMA_CH2_TIMERA1CCR0,
                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                    UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);

    //Setting Control Indexes.
    MAP_DMA_setChannelControl(UDMA_PRI_SELECT | DMA_CH2_TIMERA1CCR0,
                              UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_1);

    //Assigning/Enabling Interrupts.
    MAP_DMA_assignInterrupt(DMA_INT1, 2);
    MAP_Interrupt_enableInterrupt(DMA_INT1);

    //Now that the DMA is primed and setup, enable when ready to sample ADC Channel 6.
    //MAP_DMA_enableChannel(2);
}

/*
 * Take 1023 samples of ADC14 Channel 8 and store in array.
 *
 */
void Init_DMA_CH3(void){

    // Initialize adc data array.
    int i = 0;
    for(i = 0; i < ADC_BUFFER_LEN; i++){
        adc8_array_1[i] = 0;
    }
    adc8_array_1[0] = 2; // Element 0 is the sentinel character. 1 indicates ADC channl 6(0).

    DMA_assignChannel(DMA_CH3_TIMERA1CCR2);

    //Disabling channel attributes.
    MAP_DMA_disableChannelAttribute(DMA_CH3_TIMERA1CCR2,
                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                    UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);

    //Setting Control Indexes.
    MAP_DMA_setChannelControl(UDMA_PRI_SELECT | DMA_CH3_TIMERA1CCR2,
                              UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_1);

    //Assigning/Enabling Interrupts.
    MAP_DMA_assignInterrupt(DMA_INT2, 3);
    MAP_Interrupt_enableInterrupt(DMA_INT2);

    //Now that the DMA is primed and setup, enable when ready to sample ADC Channel 6.
    //MAP_DMA_enableChannel(3);
}

void Init_DMA_CH4(void){

    DMA_assignChannel(DMA_CH4_EUSCIA2TX);

    //Disabling channel attributes.
    MAP_DMA_disableChannelAttribute(DMA_CH4_EUSCIA2TX,
                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                    UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);

    //Setting Control Indexes.
    MAP_DMA_setChannelControl(UDMA_PRI_SELECT | DMA_CH4_EUSCIA2TX,
                              UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_1);

    //Assigning/Enabling Interrupts.
    //Do not assign interrupt. Handled by interrupt 0 automatically.

    //Channel is enabled when a write to UART2 is ready.
    //MAP_DMA_enableChannel(4);
}

void Init_DMA_CH5(void){

    DMA_assignChannel(DMA_CH5_EUSCIA2RX);

    //Disabling channel attributes.
    MAP_DMA_disableChannelAttribute(DMA_CH5_EUSCIA2RX,
                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                    UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);

    //Setting Control Indexes.
    MAP_DMA_setChannelControl(UDMA_PRI_SELECT | DMA_CH5_EUSCIA2RX,
                              UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_1);

    //Assigning/Enabling Interrupts.
    //Do not assign interrupt. Handled by interrupt 0 automatically.

    //Channel is enabled and is triggered when UART2 receives data is ready.
    MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT | DMA_CH5_EUSCIA2RX,
                                   UDMA_MODE_BASIC, (void*)UART_getReceiveBufferAddressForDMA(EUSCI_A2_BASE), command_buffer[0], 6);
    MAP_DMA_enableChannel(5);
}

/*
 * Determines if DMA0 is currently sending data to UART0. For external use.
 *
 */
uint8_t Dma_Ch0_Uart0_Busy(void){
    return dma_ch0_uart0_busy;
}

/*
 * Sets off DMA0 on sending data to UART0. Returns 1 if unsuccessful,
 * returns 0 if successful.
 *
 */
uint8_t Dma_Ch0_Uart0_Send_String(uint8_t* str_ptr, uint16_t data_len){

    if(dma_ch0_uart0_busy){
        return FAILURE;
    }
    MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT | DMA_CH0_EUSCIA0TX,
                               UDMA_MODE_BASIC, str_ptr, (void*)UART_getTransmitBufferAddressForDMA(EUSCI_A0_BASE), data_len);
    MAP_DMA_enableChannel(0);

    dma_ch0_uart0_busy = TRUE;
    return SUCCESS;
}

/*
 *  Takes a packet of data from ADC Channel 6 on DMA Channel 2.
 *  First checks if there is a measurement in progress. If so,
 *  then it returns 1. Then, determines first buffer available,
 *  if no buffers are available then it returns 2. Otherwise, it
 *  starts putting data into selected buffer and saves that buffers
 *  pointer.
 *
 */
uint8_t Dma_Ch2_ADC_Ch6_Get_Data(void){

    if(adc6_busy_1){
        return 1;
    }

    // Up here will be a way to select which buffer to use.
    adc6_active_buffer = adc6_array_1;
    adc6_busy_1 = TRUE;

    if(Check_Command(6)){
        TIMER_A1->CCR[0] = (uint16_t)Get_Command_Value(6);
        Clear_Command(6);
    }

    MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT | DMA_CH2_TIMERA1CCR0,
                               UDMA_MODE_BASIC, (void*)&(ADC14->MEM[5]), (adc6_active_buffer + 1), (ADC_BUFFER_LEN - 1));   // Send 1 byte less than the size for sentinel character.
    MAP_DMA_enableChannel(2);

    return SUCCESS;
}

/*
 *  Takes a packet of data from ADC Channel 8 on DMA Channel 3.
 *  First checks if there is a measurement in progress. If so,
 *  then it returns 1. Then, determines first buffer available,
 *  if no buffers are available then it returns 2. Otherwise, it
 *  starts putting data into selected buffer and saves that buffers
 *  pointer.
 *
 */
uint8_t Dma_Ch3_ADC_Ch8_Get_Data(void){

    if(adc8_busy_1){
        return 1;
    }

    // Up here will be a way to select which buffer to use.
    adc8_active_buffer = adc8_array_1;
    adc8_busy_1 = TRUE;

    if(Check_Command(7)){
        TIMER_A1->CCR[2] = (uint16_t)Get_Command_Value(7);
        Clear_Command(7);
    }

    MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT | DMA_CH3_TIMERA1CCR2,
                               UDMA_MODE_BASIC, (void*)&(ADC14->MEM[6]), (adc8_active_buffer + 1), (ADC_BUFFER_LEN - 1));   // Send 1 byte less than the size for sentinel character.
    MAP_DMA_enableChannel(3);

    return SUCCESS;
}

/*
 * Determines if DMA4 is currently sending data to UART2. For external use.
 *
 */
uint8_t Dma_Ch4_Uart2_Busy(void){
    return dma_ch4_uart2_busy;
}

/*
 * Sets off DMA4 on sending data to UART2. Returns 1 if unsuccessful,
 * returns 0 if successful.
 *
 */
uint8_t Dma_Ch4_Uart2_Send_String(uint8_t* str_ptr, uint16_t data_len){

    if(dma_ch4_uart2_busy){
        return FAILURE;
    }
    MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT | DMA_CH4_EUSCIA2TX,
                               UDMA_MODE_BASIC, str_ptr, (void*)UART_getTransmitBufferAddressForDMA(EUSCI_A2_BASE), data_len);
    MAP_DMA_enableChannel(4);

    dma_ch4_uart2_busy = TRUE;
    return SUCCESS;
}

/*
 * Checks if a command is ready by comparing command_buffer_read to command_buffer_index.
 * If they are the same then there are no new commands. Else, we need to copy that data
 * and increment the read index by 1, making sure to wrap around at 24.
 *
 */
uint8_t Dma_Ch5_RX2_Ready(uint8_t* dest_ptr){

    if(command_buffer_read == command_buffer_index){ // Nothing is ready.
        return 1;
    }

    int i = 0;
    for(i = 0; i < 6; i++){
        dest_ptr[i] = command_buffer[command_buffer_read][i];
    }

    if((++command_buffer_read) > 24){
        command_buffer_read = 0;   // Wrap it around.
    }

    return 0;
}

/*
 * DMA Interrupt 0 handler. Looks at INT0 flags through DMA_getInterruptStatus.
 * Checks for Channel 0 (UART0 TX), channel 4 (UART2 TX), and channel 5 (UART2 RX).
 *
 */
void DMA_INT0_IRQHandler(void){


    uint8_t temp_rx = 0;
    uint8_t i = temp_rx;
    uint8_t error_packet[2] = {5, 1};

    if(DMA_CH0_FLAG & DMA_getInterruptStatus()){    // DMA Channel 0 caused interrupt.
        DMA_clearInterruptFlag(0);  // Clear flag.
        //P2OUT ^= GPIO_PIN2;         // Toggle LED for debugging.
        dma_ch0_uart0_busy = FALSE; // DMA0 is no longer busy with UART0 TX.
    }
    if(DMA_CH4_FLAG & DMA_getInterruptStatus()){    // DMA Channel 4 caused interrupt.
        DMA_clearInterruptFlag(4);  // Clear flag.
        //P2OUT ^= GPIO_PIN1;         // Toggle LED for debugging.
        dma_ch4_uart2_busy = FALSE; // DMA4 is no longer busy with UART2 TX.
        adc6_busy_1 = FALSE;
        adc8_busy_1 = FALSE;
    }
    if(DMA_CH5_FLAG & DMA_getInterruptStatus()){    // DMA Channel 5 caused interrupt.
        DMA_clearInterruptFlag(5);
        //P2OUT ^= GPIO_PIN1;         // Toggle LED for debugging.
        // Here I need to tell UART that a command is received.
        if(command_buffer[command_buffer_index][0] != 'c'){ // Command Error. Empty RX buffer and try again.
            for(i = 0; i < 10; i++){
                temp_rx = EUSCI_A2->RXBUF;  // This should empty out the buffer.
            }
            Dma_Ch4_Uart2_Send_String(error_packet, 2);
        }
        if((++command_buffer_index) > 24){
            command_buffer_index = 0;
        }

        RX2_Command();  // Updates command flags and values.

        MAP_DMA_setChannelTransfer(UDMA_PRI_SELECT | DMA_CH5_EUSCIA2RX, //command_buffer_ptr[command_buffer_index]
                                       UDMA_MODE_BASIC, (void*)UART_getReceiveBufferAddressForDMA(EUSCI_A2_BASE), command_buffer[command_buffer_index], 6);
        MAP_DMA_enableChannel(5);
    }
}

/*
 * DMA Interrupt 1 handler. This interrupt is setup in the DMA initialization.
 * This interrupt will likely be tied to ADC Channel 1  using TimerA1 CCR0.
 *
 */
void DMA_INT1_IRQHandler(void){

    //P2OUT  ^= GPIO_PIN2;
    //adc6_busy_1 = FALSE;
    Dma_Ch4_Uart2_Send_String(adc6_active_buffer, ADC_BUFFER_LEN);
}

/*
 * DMA Interrupt 2 handler. This interrupts are setup in the DMA initialization.
 * This interrupt will likely be tied to ADC Channel 2 using TimerA1 CCR2.
 *
 */
void DMA_INT2_IRQHandler(void){

    //P2OUT  ^= GPIO_PIN2;
    //adc8_busy_1 = FALSE;
    Dma_Ch4_Uart2_Send_String(adc8_active_buffer, ADC_BUFFER_LEN);
}

/*
 * DMA Interrupt 3 handler. This interrupts are setup in the DMA initialization.
 */
void DMA_INT3_IRQHandler(void){

    // Currently not configured.
}
