/*
 * Data_Handling.c
 *
 *  Created on: Nov 8, 2024
 *      Author: GIGABYTE
 */

/*
 * @note: written by chat gpt
 */
void floatToString(float num, char *str, int precision) {
    int intPart = (int)num;  // Get the integer part of the float
    float fracPart = num - intPart;  // Get the fractional part

    // Convert the integer part to string
    int index = 0;
    if (intPart == 0 && num < 0) {
        str[index++] = '-';
    }
    int intPartCopy = intPart;
    int intPartRev = 0;

    // Reverse the integer part digits
    while (intPartCopy != 0) {
        intPartRev = intPartRev * 10 + intPartCopy % 10;
        intPartCopy /= 10;
    }

    // Store the integer part in the string
    while (intPartRev != 0) {
        str[index++] = intPartRev % 10 + '0';
        intPartRev /= 10;
    }

    if (index == 0) {
        str[index++] = '0';  // Handle the case where the number is 0
    }

    // Add the decimal point
    str[index++] = '.';

    // Convert the fractional part to string
    for (int i = 0; i < precision; i++) {
        fracPart *= 10;
        int fracDigit = (int)fracPart;
        str[index++] = fracDigit + '0';  // Convert to character
        fracPart -= fracDigit;  // Remove the integer part
    }

    // Null-terminate the string
    str[index] = '\0';
}
