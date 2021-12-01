/*
 * oled1306.c
 *
 *  Created on: Sep 18, 2021
 *      Author: willi
 */

#include "oled1306.h"
#include "macros.h"


/*
 *
 */
void OLED_Init(void){

    unsigned char info[] = {0x00, 0xAE};
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0xD5;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0x80;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0xA8;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0x3F;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0xD3;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0x00;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0x40;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0x8D;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0x14;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0x20;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0x00;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0xA1;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0xC8;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0xDA;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0x12;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0x81;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0x80;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0xD9;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0xF1;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0xDB;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0x20;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0xA4;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0xA6;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0x2E;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    info[0] = 0x00;
    info[1] = 0xAF;
    while(I2C1_TX_RX(0x3C, info, 2, FALSE));
}

/*
 *
 */
void OLED_SetXY(char col_start, char col_end, char page_start, char page_end){

    unsigned char info[7] = {0x00, 0x21, col_start, col_end, 0x22, page_start, page_end};
    while(I2C1_TX_RX(0x3C, info, 7, FALSE));

}

/*
 *
 */
void OLED_Clear(void){

    unsigned char info[] = {0x40, 0x00};
    OLED_SetXY(0x00, 0x7F, 0x00, 0x07);

    int k = 0;
    for(k = 0; k <= 1023; k++){
        while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    }
}

/*
 *
 */
void OLED_Data(char *DATA, int strLen){

    int g = 0;
    int index = 0;
    unsigned char info[] = {0x40, 0x00};

    for(g = 0; g < strLen; g++){

        for(index = 0; index < 5; index++){
            info[1] = ASCII[DATA[g] - 0x20][index];
            while(I2C1_TX_RX(0x3C, info, 2, FALSE));
        }
    }
}

/*
 *
 */
void OLED_Printline(char line, char* DATA, int length){

    OLED_SetXY(0x00, 0x7F, (line+1), line);
    OLED_Data(DATA, length);
}


/*
 * Prints the image given by the variable ti_logo. Can be replaced with
 * other image data if you want.
 */
void OLED_Logo_TI(void){

    int i = 0;
    unsigned char info[] = {0x40, 0x00};
    OLED_SetXY(0x00, 0x7F, 0x00, 0x07);

    for(i = 0; i < 1024; i++){
        info[1] = ti_logo[i];
        while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    }
}

void OLED_Logo_NCSU(void){

    int i = 0;
    unsigned char info[] = {0x40, 0x00};
    OLED_SetXY(0x00, 0x7F, 0x00, 0x07);

    for(i = 0; i < 1024; i++){
        info[1] = ncsu_logo[i];
        while(I2C1_TX_RX(0x3C, info, 2, FALSE));
    }
}
