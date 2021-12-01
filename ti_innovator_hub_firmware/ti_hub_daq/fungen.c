/*
 * fungen.c
 *
 *  Description: The function generator is updated via an wave template
 *  that is modified by the active properties global variables. When a
 *  command is received to change a properties, the function generator
 *  is temporarily disabled and the active_signal array is updated with
 *  a modified template. Some properties are stored as modified values
 *  to prevent floating points. The calculation for the 16 bit value
 *  is calculated via the GUI and sent over to be stored as a parameter.
 *  This simplifies calculations in the MSP432. Timing modifications are
 *  handled in the timer.c file.
 *
 *  Command Mapping:
 *       9 - Type Ch1, 10 - Type Ch2, 11 - Type Ch3, 12 - Type Ch4
 *      13 - Amp  Ch1, 14 - Amp  Ch2, 15 - Amp  Ch3, 16 - Amp  Ch4
 *      17 - Freq Ch1, 18 - Freq Ch2, 19 - Freq Ch3, 20 - Freq Ch4
 *      21 - Bias Ch1, 22 - Bias Ch2, 23 - Bias Ch3, 24 - Bias Ch4
 *      25 - Duty Ch1, 26 - Duty Ch2, 27 - Duty Ch3, 28 - Duty Ch4
 *
 */

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "i2c_ti82.h"
#include "macros.h"
#include "fungen.h"
#include "timers.h"
#include "uart_ti82.h"

// Default signal is the sine wave template.
uint16_t active_signal[128] = {20346, 20832, 21316, 21796, 22272, 22741, 23204, 23659, 24105, 24540, 24964, 25376, 25774, 26158, 26527, 26880,
                               27216, 27535, 27835, 28115, 28376, 28616, 28835, 29033, 29208, 29361, 29491, 29598, 29681, 29741, 29777, 29789,
                               29777, 29741, 29681, 29598, 29491, 29361, 29208, 29033, 28835, 28616, 28376, 28115, 27835, 27535, 27216, 26880,
                               26527, 26158, 25774, 25376, 24964, 24540, 24105, 23659, 23204, 22741, 22272, 21796, 21316, 20832, 20346, 19859,
                               19372, 18886, 18402, 17922, 17446, 16977, 16514, 16059, 15614, 15178, 14754, 14343, 13944, 13560, 13191, 12838,
                               12502, 12183, 11884, 11603, 11342, 11102, 10883, 10685, 10510, 10357, 10227, 10120, 10037,  9977,  9942,  9930,
                                9942,  9977, 10037, 10120, 10227, 10357, 10510, 10685, 10883, 11102, 11342, 11603, 11884, 12183, 12502, 12838,
                               13191, 13560, 13944, 14343, 14754, 15178, 15614, 16059, 16514, 16977, 17446, 17922, 18402, 18886, 19372, 19859};

// Current active properties of the generated waveform.
uint16_t active_type = 1;
uint16_t active_amp  = 1;
uint16_t active_freq = 2;
uint16_t active_bias = 1;
uint8_t active_channel = CH_OFF; // Default is channel A

uint16_t fun_index = 0;

// 19859 is the anchor point for scaling and offset calculations.
uint16_t sine_template[128]     = {20346, 20832, 21316, 21796, 22272, 22741, 23204, 23659, 24105, 24540, 24964, 25376, 25774, 26158, 26527, 26880,
                                   27216, 27535, 27835, 28115, 28376, 28616, 28835, 29033, 29208, 29361, 29491, 29598, 29681, 29741, 29777, 29789,
                                   29777, 29741, 29681, 29598, 29491, 29361, 29208, 29033, 28835, 28616, 28376, 28115, 27835, 27535, 27216, 26880,
                                   26527, 26158, 25774, 25376, 24964, 24540, 24105, 23659, 23204, 22741, 22272, 21796, 21316, 20832, 20346, 19859,
                                   19372, 18886, 18402, 17922, 17446, 16977, 16514, 16059, 15614, 15178, 14754, 14343, 13944, 13560, 13191, 12838,
                                   12502, 12183, 11884, 11603, 11342, 11102, 10883, 10685, 10510, 10357, 10227, 10120, 10037,  9977,  9942,  9930,
                                    9942,  9977, 10037, 10120, 10227, 10357, 10510, 10685, 10883, 11102, 11342, 11603, 11884, 12183, 12502, 12838,
                                   13191, 13560, 13944, 14343, 14754, 15178, 15614, 16059, 16514, 16977, 17446, 17922, 18402, 18886, 19372, 19859};

uint16_t square_template[128]   = {29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842,
                                   29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842,
                                   29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842,
                                   29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842, 29842,
                                    9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,
                                    9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,
                                    9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,
                                    9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966,  9966};

uint16_t saw_template[128]      = { 9948, 10102, 10258, 10412, 10568, 10722, 10878, 11032, 11188, 11342, 11498, 11652, 11808, 11962, 12118, 12272,
                                   12428, 12582, 12738, 12892, 13048, 13202, 13358, 13512, 13668, 13822, 13978, 14132, 14288, 14442, 14598, 14752,
                                   14908, 15062, 15218, 15372, 15528, 15682, 15838, 15992, 16148, 16302, 16458, 16612, 16768, 16922, 17078, 17232,
                                   17388, 17542, 17698, 17852, 18008, 18162, 18318, 18472, 18628, 18782, 18938, 19092, 19248, 19402, 19558, 19712,
                                   19868, 20022, 20178, 20332, 20488, 20642, 20798, 20952, 21108, 21262, 21418, 21572, 21728, 21882, 22038, 22192,
                                   22348, 22502, 22658, 22812, 22968, 23122, 23278, 23432, 23588, 23742, 23898, 24052, 24208, 24362, 24518, 24672,
                                   24828, 24982, 25138, 25292, 25448, 25602, 25758, 25912, 26068, 26222, 26378, 26532, 26688, 26842, 26998, 27152,
                                   27308, 27462, 27618, 27772, 27928, 28082, 28238, 28392, 28548, 28702, 28858, 29012, 29168, 29322, 29478, 29632};

/*
 * The initial function generator active properties are set.
 *
 */
void Init_FunGen(void){

    int i = 0;
    for(i = 0; i < 4; i++){
        Set_Command_Value(9 + i, 0);
        Set_Command_Value(13+ i, 19859);
        Set_Command_Value(21+ i, 19859);
        Set_Command_Value(17+ i,  5859);
    }
}


/*
 * Sends next signal value to the DAC via the actively selected channel.
 *
 */
uint8_t FunGen_Process(void){

    if((active_channel == CH_OFF) || !Is_I2C_Ready()){
        return 1;
    }
    if(active_channel & 0x20){
        uint8_t packet[3] = {(active_channel & 0x1F), 0x00, 0x00};
        I2C1_TX_RX(0x4C, packet, 3, FALSE);
    } else{
        uint8_t packet[3] = {active_channel, (active_signal[fun_index] >> 8), active_signal[fun_index]};
        I2C1_TX_RX(0x4C, packet, 3, FALSE);
        fun_index = (fun_index + 1) % 128;
    }
    return 0;
}

/*
 * A command with a new property value was received. We need to stop the
 * function generator and update the active_signal array.
 *
 */
void Update_Signal_Array(void){

    TIMER_A0->CCTL[0] &= ~0x10;      // Disable compare value interrupt for channel 0

    Set_Active_Channel(Get_Command_Value(5));
    int16_t channel_value = ((0x1F & Get_Command_Value(5)) / 2) - 8;
    if(channel_value < 0){
        return;
    }
    Set_Active_Type(Get_Command_Value(9 + channel_value));
    Set_Active_Bias(Get_Command_Value(21 + channel_value));
    Set_Active_Amp(Get_Command_Value(13 + channel_value));
    Set_Timer_Frequency(Get_Command_Value(17 + channel_value));

    uint16_t i = 0;
    int32_t temp = 0;

    // Compiler raises warning if declared inside switch statement.
    int32_t bias = 0;
    float amp_scale = 0.0;
    int32_t amp = 0;

    switch(active_type){
    case 0:                 // Square Wave
        bias = active_bias - 19895;
        amp_scale = active_amp*(3.3/65535);
        amp  = 0;
        for(i = 0; i < 128; i++){
            amp  = (int32_t)((amp_scale*2*(square_template[i] - 19895))/2);
            temp = 19895 + bias + amp;
            if(temp > 65535){
                temp = 65535;
            } else if(temp < 0){
                temp = 0;
            }
            active_signal[i] = temp;
        }
        break;
    case 1:                 // Sine Wave is option 1.
        bias = active_bias - 19895;
        amp_scale = active_amp*(3.3/65535);
        amp  = 0;
        for(i = 0; i < 128; i++){
            amp  = (int32_t)((amp_scale*2*(sine_template[i] - 19895))/2);
            temp = 19895 + bias + amp;
            if(temp > 65535){
                temp = 65535;
            } else if(temp < 0){
                temp = 0;
            }
            active_signal[i] = temp;
        }
        break;
    case 2:
        bias = active_bias - 19895;
        amp_scale = active_amp*(3.3/65535);
        amp  = 0;
        for(i = 0; i < 128; i++){
            amp  = (int32_t)((amp_scale*2*(saw_template[i] - 19895))/2);
            temp = 19895 + bias + amp;
            if(temp > 65535){
                temp = 65535;
            } else if(temp < 0){
                temp = 0;
            }
            active_signal[i] = temp;
        }
        break;
    case 3:                 // DC Wave
        for(i = 0; i < 128; i++){
            active_signal[i] = active_bias;
        }
        // Add code to set longer period for DC.
        break;
    default:
        break;
    }

    TIMER_A0->CCTL[0] |=  0x10;      // Enable compare value interrupt for channel 0
}


/*
 * These functions are to allow setting the active property.
 *
 */
void Set_Active_Channel(uint8_t value){
    active_channel = value;
}

void Set_Active_Type(uint16_t value){
    active_type = value;
}

void Set_Active_Bias(float value){

    active_bias = (uint16_t)(value);
}

void Set_Active_Amp(float value){

    active_amp  = (uint16_t)(value);
}
