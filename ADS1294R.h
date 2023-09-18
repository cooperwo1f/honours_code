#ifndef _ADS1294R_H_
#define _ADS1294R_H_

#include <xc.h>
#include <stdint.h>

#include "util.h"
#include "ESP32.h"


/* Pin Mapping */

// Test points
#define TP6_PIN PORTDbits.RD4
#define TP7_PIN PORTDbits.RD5
#define TP8_PIN PORTDbits.RD6

// Controls
#define DRDY_PIN PORTDbits.RD7
#define CLKSEL_PIN PORTDbits.RD8
#define CS_PIN PORTDbits.RD9
#define START_PIN PORTDbits.RD10


/* Register Addresses */

// Device settings (READ-ONLY)
#define ID          0x00

// Global Settings across channels
#define CONFIG1     0x01
#define CONFIG2     0x02
#define CONFIG3     0x03
#define LOFF        0x04

// Channel-specific settings
#define CH1SET      0x05
#define CH2SET      0x06
#define CH3SET      0x07
#define CH4SET      0x08
#define RLD_SENSP   0x0D
#define RLD_SENSN   0x0E
#define LOFF_SENSP  0x0F
#define LOFF_SENSN  0x10
#define LOFF_FLIP   0x11

// Lead-off status registers (READ-ONLY)
#define LOFF_STATP  0x12
#define LOFF_STATN  0x13

// GPIO and other registers
#define GPIO        0x14
#define PACE        0x15
#define RESP        0x16
#define CONFIG4     0x17
#define WCT1        0x18
#define WCT2        0x19


/* SPI Command Definitions */

// System commands
#define WAKEUP      0x02
#define STANDBY     0x04
#define RESET       0x06
#define START       0x08
#define STOP        0x0A

// Data read commands
#define RDATAC      0x10
#define SDATAC      0x11
#define RDATA       0x12


/* Chip info */

// Channel definitions
#define NUMBER_OF_CHANNELS 4
#define BYTES_PER_CHANNEL 3
#define BYTES_TO_READ (NUMBER_OF_CHANNELS * BYTES_PER_CHANNEL)

#define CS_DELAY 0

/* Low-level driver */

uint8_t ADS1294R_write(uint8_t data) {
    // Low-level SPI driver
    SPI3BUF = (uint32_t)data;           // Place data we want to send in SPI buffer
    while(!SPI3STATbits.SPITBE);        // Wait until sent status bit is cleared
    return (uint8_t)SPI3BUF;            // Read data from buffer to clear it
}

uint8_t ADS1294R_read() {
    return ADS1294R_write(0x00);
}

void write_cmd(uint8_t cmd) {
    CS_PIN = 0;
    ADS1294R_write(cmd);
    CS_PIN = 1;
}

/* Register drivers */

uint8_t read_register(uint8_t reg) {
    static uint8_t read_register_cmd = 0x20;
    static uint8_t read_register_mask = 0x1F;
    
    uint8_t first_byte = read_register_cmd | (reg & read_register_mask);
    uint8_t second_byte = 0x00; // only ever read a single register
    
    CS_PIN = 0;
    ADS1294R_write(first_byte);
    ADS1294R_write(second_byte);
    ADS1294R_read();
    uint8_t ret = ADS1294R_read();
    CS_PIN = 1;

    return ret;
}

void write_register(uint8_t reg, uint8_t data) {
    static uint8_t write_register_cmd = 0x40;
    static uint8_t write_register_mask = 0x1F;
    
    uint8_t first_byte = write_register_cmd | (reg & write_register_mask);
    uint8_t second_byte = 0x00; // only ever write a single register
    
    CS_PIN = 0;
    ADS1294R_write(first_byte);
    ADS1294R_write(second_byte);
    ADS1294R_write(data);
    CS_PIN = 1;
}


/* ADS1298RADS1294R init */

void ADS1294R_GPIO_init() {
    // Not sure if any of these work...
    TRISDbits.TRISD4 = 0;       // TP6 as output    -  Pin 52
    TRISDbits.TRISD5 = 0;       // TP7 as output    -  Pin 53
    TRISDbits.TRISD6 = 0;       // TP8 as output    -  Pin 54
    
    TRISDbits.TRISD7 = 1;       // nDRDY as input   -  Pin 55
    TRISDbits.TRISD8 = 0;       // CLKSEL as output -  Pin 42
    TRISDbits.TRISD9 = 0;       // CS as output     -  Pin 43
    TRISDbits.TRISD10 = 0;      // START as output  -  Pin 44
    
    TP6_PIN = 0;
    TP7_PIN = 0;
    TP8_PIN = 0;
    
    CLKSEL_PIN = 0;
    CS_PIN = 1;
    START_PIN = 0;
}

void ADS1294R_SPI_init() {
    SPI3CONbits.ON = 0;         // Turn off SPI2 before configuring
    SPI3CONbits.FRMEN = 0;      // Framed SPI Support (SS pin used)
    SPI3CONbits.MSSEN = 0;      // Slave Select Enable (SS driven during transmission)
    SPI3CONbits.ENHBUF = 0;     // Enhanced Buffer Enable (disable enhanced buffer)
    SPI3CONbits.SIDL = 1;       // Stop in Idle Mode
    SPI3CONbits.DISSDO = 0;     // Disable SDOx (pin is controlled by this module)
    SPI3CONbits.MODE32 = 0;     // Do not use 32-bit mode (8-bit mode)
    SPI3CONbits.MODE16 = 0;     // Do not use 16-bit mode (8-bit mode)
    
    // SMP = 1; data sampled at end of output time... SMP = 0; data sampled at middle of output time
    SPI3CONbits.SMP = 1;
    
    // CKE = 1; transition from active to idle... CKE = 0; transition from idle to active
    SPI3CONbits.CKE = 0;
    
    // CKP = 1; high is idle, low is active... CKP = 0; low is idle, high is active
    SPI3CONbits.CKP = 0;
    
    SPI3CONbits.SSEN = 0;       // Slave Select Enable (SS pin used by module)
    SPI3CONbits.MSTEN = 1;      // Master Mode Enable
    SPI3CONbits.STXISEL = 0b01; // SPI Transmit Buffer Empty Interrupt Mode (generated when the buffer is completely empty)
    SPI3CONbits.SRXISEL = 0b11; // SPI Receive Buffer Full Interrupt Mode (generated when the buffer is full)
    
    // SCLK period > 70ns
    // 70ns ~= 14.3MHz
    // F_SCK = 14MHz
    
    // Library uses 4MHz
    
    // BRG = (F_PB / 2 * F_SCK) - 1
    // BRG = 1.86
    // BRG >= 2

    SPI3BRG = 4;
    SPI3CONbits.ON = 1;         // Configuration is done, turn on SPI3 peripheral
}


/* Public Functions */

void ADS1294R_init() {
    ADS1294R_GPIO_init();
    ADS1294R_SPI_init();

    // Set CLKSEL pin = 1
    CLKSEL_PIN = 1;
    delay(1);
    
    write_cmd(RESET);
    delay(1);

    // Send Stop Data Continuous command
    write_cmd(SDATAC);
    delay(1);
    
//    write_register(GPIO, 0b11110000);

    // Write config registers
    write_register(CONFIG1, 0x86);  // 500 samples/s
    delay(1);
//    debug("CONFIG1: expected 0xC6 actual 0x%02x", read_register(CONFIG1));
    write_register(CONFIG2, 0x00);  // Test signals disabled
    delay(1);
//    debug("CONFIG2: expected 0x00 actual 0x%02x", read_register(CONFIG2));
    write_register(CONFIG3, 0xC0);  // Enable internal reference buffer, no RLD
    delay(1);
//    debug("CONFIG3: expected 0xC0 actual 0x%02x", read_register(CONFIG3));

    // Send Read Data Continuous command
//    ADS1294R_write(START);
//    delay(1);
//    ADS1294R_write(RDATAC);
    delay(1);
//    START_PIN = 1;
}

uint8_t read_data() {
    return ADS1294R_read();
}

uint8_t data_ready() {
    return DRDY_PIN == 0;
}

#endif /* _ADS1294R_H_ */