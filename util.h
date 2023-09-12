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

#endif // _UTIL_H_
