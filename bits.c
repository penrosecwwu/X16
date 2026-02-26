#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "bits.h"



// Get the nth bit
uint16_t getbit(uint16_t number, int n) {
    // create a mask with only nth bit set to 1
    uint16_t mask = (uint16_t)1 << n;

    // extract the bit using AND and right shift
    uint16_t extracted_bit = (number & mask) >> n;

    // return extracted bit
    return extracted_bit;
}

// Get bits that are the given number of bits wide
uint16_t getbits(uint16_t number, int n, int wide) {
    // create the bounds
    uint16_t start = n;
    uint16_t end = n+wide-1;

    // create the mask
    uint16_t mask = (uint16_t)0;
    for (int i = start; i <= end; i++){
        mask = mask | ((uint16_t)1 << i);
    }
    // extract the bits
    uint16_t extracted_bits = (number & mask) >> n;
    // return extracted bit
    return extracted_bits;
}

// Set the nth bit to the given bit value and return the result
// return number with the nth bit set to 1
uint16_t setbit(uint16_t number, int n) {
    // create mask
    uint16_t mask = 1 << n;
    // set to 1
    number = number | mask;
    // return number
    return number;
}

// Clear the nth bit
uint16_t clearbit(uint16_t number, int n) {
    uint16_t mask = ~((uint16_t)1 << n);
    number = number & mask;
    return number;
}

// Sign extend a number of the given bits to 16 bits
uint16_t sign_extend(uint16_t x, int bit_count) {
    int16_t temp = (int16_t)(x << (16-bit_count));
    temp = temp >> (16-bit_count);
    return (uint16_t)temp;
}

bool is_positive(uint16_t number) {
    return getbit(number, 15) == 0;
}

bool is_negative(uint16_t number) {
    return getbit(number, 15) == 1;
}
