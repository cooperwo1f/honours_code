#ifndef _ESP32_H_
#define _ESP32_H_

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

uint32_t ESP32_SPI_write(uint32_t data) {
    // Low-level SPI driver
    SPI2BUF = data;                 // Place data we want to send in SPI buffer
    while(!SPI2STATbits.SPITBE);    // Wait until sent status bit is cleared
    return SPI2BUF;                 // Read data from buffer to clear it
}

void ESP32_SPI_write_4byte(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4) {
    uint32_t word = ((uint32_t)b1 << 24) | ((uint32_t)b2 << 16) | ((uint32_t)b3 << 8) | (uint32_t)b4;
    ESP32_SPI_write(word);
}

void ESP32_SPI_write8(uint8_t data) {
    ESP32_SPI_write_4byte(data, 0, 0, 0);
    delay(5);
}

void ESP32_SPI_write_array(uint8_t *array, size_t len) {
    for (size_t i = 0; i < len; i++) {
        ESP32_SPI_write8(array[i]);
    }
}

void write_packet(uint8_t* buf, size_t len) {
    uint8_t mod_table[] = {0, 2, 1};
    char encoding_table[] = {   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'
    };

    size_t output_length = 4 * ((len + 2) / 3);
    char encoded_data[output_length];

    for (int i = 0, j = 0; i < len;) {
        uint32_t octet_a = i < len ? buf[i++] : 0;
        uint32_t octet_b = i < len ? buf[i++] : 0;
        uint32_t octet_c = i < len ? buf[i++] : 0;

        uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

        encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
        encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
    }

    for (int i = 0; i < mod_table[len % 3]; i++) {
        encoded_data[output_length - 1 - i] = '=';
    }

    ESP32_SPI_write_array(encoded_data, output_length);
    ESP32_SPI_write8('\n');
}

void debug(const char *fmt, ...) {
    va_list args;
    char str[1024];

    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);

    write_packet(str, strlen(str));
}

void ESP32_IO_init() {
    TRISBbits.TRISB2 = 0;       // Set ESP32 EN pin as output
    PORTBbits.RB2 = 1;          // Set ESP32 EN pin high
}

void ESP32_SPI_init() {
    SPI2CONbits.ON = 0;         // Turn off SPI2 before configuring
    SPI2CONbits.FRMEN = 0;      // Framed SPI Support (SS pin used)
    SPI2CONbits.FRMSYNC = 0;    // Frame Sync Pulse Direction (pulse output, master mode)
    SPI2CONbits.FRMPOL = 0;     // Slave Select Polarity (SS is active low)
    SPI2CONbits.MSSEN = 1;      // Slave Select Enable (SS driven during transmission)
    SPI2CONbits.FRMSYPW = 1;    // Frame Sync Pulse Width (frame sync pulse is one clock wide)
    SPI2CONbits.FRMCNT = 0b101; // Frame Sync Pulse Counter (pulse on every data character)
    SPI2CONbits.SPIFE = 0;      // Frame Sync Pulse Edge Select (frame sync precedes first bit)
    SPI2CONbits.ENHBUF = 0;     // Enhanced Buffer Enable (disable enhanced buffer)
    SPI2CONbits.SIDL = 1;       // Stop in Idle Mode
    SPI2CONbits.DISSDO = 0;     // Disable SDOx (pin is controlled by this module)
    SPI2CONbits.MODE32 = 1;     // Use 32-bit mode
    SPI2CONbits.MODE16 = 0;     // Do not use 16-bit mode
    SPI2CONbits.SMP = 0;        // Input data is sampled at the end of the clock signal
    SPI2CONbits.CKE = 1;        // Data is shifted out/in on transition from idle (high) state to active (low) state
    SPI2CONbits.SSEN = 1;       // Slave Select Enable (SS pin used by module)
    SPI2CONbits.CKP = 0;        // Clock Polarity Select (clock signal is active low, idle state is high)
    SPI2CONbits.MSTEN = 1;      // Master Mode Enable
    SPI2CONbits.STXISEL = 0b01; // SPI Transmit Buffer Empty Interrupt Mode (generated when the buffer is completely empty)
    SPI2CONbits.SRXISEL = 0b11; // SPI Receive Buffer Full Interrupt Mode (generated when the buffer is full)
    SPI2BRG = 50;
    SPI2CONbits.ON = 1;         // Configuration is done, turn on SPI2 peripheral
}

void ESP32_init() {
    ESP32_IO_init();
    ESP32_SPI_init();
}

#endif /* _ESP32_H_ */