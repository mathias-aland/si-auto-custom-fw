#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "rs232.h"

uint8_t getParity (uint8_t n) {
    uint8_t p = 0;
    
    if(n & 0x1)
        p++;
    if(n & 0x2)
        p++;
    if(n & 0x4)
        p++;
    if(n & 0x8)
        p++;
    if(n & 0x10)
        p++;
    if(n & 0x20)
        p++;
    if(n & 0x40)
        p++;
            
    if (p & 0x1)
    {
        // odd number, parity bit = 1
        n |= 0x80;
    }
    
    return n;
}


void rs232TxByte(uint8_t byte)
{
    byte = getParity(byte); // Calculate parity
    TX1REG = byte;
}

uint8_t rs232RxByte()
{
    uint8_t dat = RC1REG;
    bool par_rx;
    bool par;
    
    if (dat & 0x80)
    {
        par_rx = 1;
    }
    else
    {
        par_rx = 0;
    }
    
    dat &= ~0x80;  // Zero out parity bit
    
    // Calculate parity
    
    if (getParity(dat) & 0x80)
    {
        par = 1;
    }
    else
    {
        par = 0;
    }
    
    if (par != par_rx)
    {
        // Parity error
        dat |= 0x80;
    }
    
    return dat;
}
