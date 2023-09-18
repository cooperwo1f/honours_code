#ifndef _UTIL_H_
#define _UTIL_H_

#include <xc.h>

// SYS_FREQ = CRYSTAL_FREQ / 10 * 16 / 8
//#define SYS_FREQ 5000000

// May not be exactly actually since scope says different but close enough
#define DELAY_CONST 2.5

void delay_us(unsigned int us) {
    // Convert microseconds us into how many clock ticks it will take
    // Doing this causes an overflow I think... better to precalc and put in magic number
//    us *= SYS_FREQ / 1000000 / 2;   // Core Timer updates every 2 ticks
    us *= DELAY_CONST;
    _CP0_SET_COUNT(0);              // Set Core Timer count to 0
    while (us > _CP0_GET_COUNT());  // Wait until Core Timer count reaches the number we calculated earlier
}

void delay_ms(int ms) {
    delay_us(ms * 1000);
}

void delay(int ms) {
    delay_ms(ms);
}

void ADC_init() {
    AD1CON1bits.ADSIDL = 0;
    AD1CON1bits.SIDL = 0;
    AD1CON1bits.ASAM = 1;   // auto sampling
    AD1CON1bits.CLRASAM = 0; // overwrite buffer
    AD1CON1bits.FORM = 0b000; // integer 16-bit output
    AD1CON1bits.SSRC = 0b111; // auto convert
    AD1CON1bits.ADON = 1;
    AD1CON1bits.ON = 1;
    AD1CON1bits.SAMP = 1;
    
    AD1CHSbits.CH0SA = 0b1111;
    AD1CHSbits.CH0NA = 0;
    AD1CHSbits.CH0SB = 0b0000;
    AD1CHSbits.CH0NB = 0;
}
#endif // _UTIL_H_
