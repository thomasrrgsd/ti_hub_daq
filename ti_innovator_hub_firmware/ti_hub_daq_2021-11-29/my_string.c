/*
 * Description: Custom string operations.
 *
 * Date: 09 Sep 2021
 *
 * Contents:
 *      - reverse   :: Takes a string and reverses it.
 *      - intToStr  :: Converts integer to string.
 *      - ftoa      :: Converts floating point to string.
 *
 *
 */

#include "macros.h"
#include "uart_ti82.h"

void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

/*void ftoa(float n, char* res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}*/


void HexToStr(int hexVal){
    char hexStr[9];
    int  tempVal = 0;
    int i = 0;
    int j = 0;

    for(i = 0; i < 9; i++){
        hexStr[i] = '\0';
    }

    hexStr[0] = '0';
    hexStr[1] = 'x';

    j = 2;
    for(i = 3; i >= 0; i--){
        tempVal = (hexVal >> (4*i)) & 0xF;
        if(tempVal <= 9){
            hexStr[j++] = tempVal + 48;
        }
        else{
            hexStr[j++] = tempVal + 55;
        }
    }
    hexStr[6] = 'p';
    hexStr[7] = '\r';
    hexStr[8] = '\n';
    TX_Debug(hexStr);
    //TX_Esp(hexStr);
}
