#ifndef UTIL_H_
#define UTIL_H_

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

void delay_us(unsigned int us) {
    // Convert microseconds us into how many clock ticks it will take
    us *= SYS_FREQ / 1000000 / 2;   // Core Timer updates every 2 ticks
    _CP0_SET_COUNT(0);              // Set Core Timer count to 0
    while (us > _CP0_GET_COUNT());  // Wait until Core Timer count reaches the number we calculated earlier
}

void delay_ms(int ms) {
    delay_us(ms * 1000);
}

uint8_t SPI_write(uint8_t data) {
    // Low-level SPI driver
    SPI2BUF = (uint32_t)data;       // Place data we want to send in SPI buffer
    while(!SPI2STATbits.SPITBE);    // Wait until sent status bit is cleared
    return (uint8_t)SPI2BUF;        // Return data that was just sent
}

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif // UTIL_H_
