/**
 * @file uca0uart.c
 * @author Marcus Behel (mgbehel@ncsu.edu)
 */


#include <msp430helper.h>

////////////////////////////////////////////////////////////////////////////////
/// Helper functions
////////////////////////////////////////////////////////////////////////////////

/**
 * Integer to string representation
 * @param value Value to convert to string
 * @param data String to write into. MUST BE AT LEAST 12 CHARS WIDE
 * @return Index of the null terminator in the string (string length)
 */
unsigned int int_to_str(int32_t value, char *data){
    // Max signed 32-bit int is 2147483647 = 10 digits + 1 for sign = 11 chars + 1 for null
    data[0] = '+';
    if(value < 0){
        data[0] = '-';
        value *= -1;
    }

    // Determine how many digits in the given value
    unsigned int numdig;
    if(value >= 1000000000) numdig = 10;
    else if(value >= 100000000) numdig = 9;
    else if(value >= 10000000) numdig = 8;
    else if(value >= 1000000) numdig = 7;
    else if(value >= 100000) numdig = 6;
    else if(value >= 10000) numdig = 5;
    else if(value >= 1000) numdig = 4;
    else if(value >= 100) numdig = 3;
    else if(value >= 10) numdig = 2;
    else numdig = 1;

    // Get ascii value of each digit & store in string
    unsigned int i;
    uint32_t q, r;
    for(i = 0; i < numdig; ++i){
        udiv10(value, &q, &r);
        value = q;
        data[numdig - i] = '0' + r;
    }
    data[numdig + 1] = '\0';
    return numdig + 1;
}

void udiv10(uint32_t n, uint32_t *q, uint32_t *r) {
    *q = (n >> 1) + (n >> 2);
    *q = *q + (*q >> 4);
    *q = *q + (*q >> 8);
    *q = *q + (*q >> 16);
    *q = *q >> 3;
    *r = n - ((*q << 3) + (*q << 1));
    *q = *q + ((*r + 6) >> 4);
    if(*r >= 10) *r -= 10;
}

