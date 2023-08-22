#ifndef _UTIL_H_
#define _UTIL_H_

#include <xc.h>

// Crystal is 25MHz / 5 * 16 = 80MHz
#define SYS_FREQ 80000000

void delay_us(unsigned int us) {
    // Convert microseconds us into how many clock ticks it will take
    us *= SYS_FREQ / 1000000 / 2;   // Core Timer updates every 2 ticks
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
