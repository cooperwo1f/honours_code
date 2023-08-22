#ifndef _ADS1298R_H_
#define _ADS1298R_H_

#include <xc.h>
#include <stdint.h>

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
#define CH5SET      0x09
#define CH6SET      0x0A
#define CH7SET      0x0B
#define CH8SET      0x0C
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
#define WAKEUP 0x02
#define STANDBY 0x04
#define RESET 0x06
#define START 0x08
#define STOP 0x0A

// Data read commands
#define RDATAC 0x10
#define SDATAC 0x11
#define RDATA 0x12


/* Low-level driver */

uint8_t ADS1298R_write(uint8_t data) {
    SPI3BUF = (uint32_t)data;       // Place data we want to send in SPI buffer
    while(!SPI3STATbits.SPITBE);    // Wait until sent status bit is cleared
    return (uint8_t)SPI3BUF;        // Read data from buffer to clear it
}


/* Register drivers */

uint8_t read_register(uint8_t reg) {
    if (reg > 0b11111) { return 0; /* out of range */ }
    
    uint8_t first_byte = 0b00100000 | reg;
    uint8_t second_byte = 0b00000000;
    
    ADS1298R_write(first_byte);
    ADS1298R_write(second_byte);
    
    uint8_t value = ADS1298R_write(0);
    return value;
}

void write_register(uint8_t reg, uint8_t data) {
    if (reg > 0b11111) { return; /* out of range */ }
    
    uint8_t first_byte = 0b01000000 | reg;
    uint8_t second_byte = 0b00000000;
    
    ADS1298R_write(first_byte);
    ADS1298R_write(second_byte);
    
    ADS1298R_write(data);
}


/* ADS1298RADS1298R init */

void ADS1298R_GPIO_init() {
    TRISDbits.TRISD7 = 1;       // nDRDY as input
    TRISDbits.TRISD8 = 0;       // CLKSEL as output
    TRISDbits.TRISD10 = 0;      // START as output
}

void ADS1298R_SPI_init() {
    SPI3CONbits.ON = 0;         // Turn off SPI3 before configuring
    SPI3CONbits.FRMEN = 0;      // Framed SPI Support (SS pin used)
    SPI3CONbits.FRMSYNC = 0;    // Frame Sync Pulse Direction (pulse output, master mode)
    SPI3CONbits.FRMPOL = 0;     // Slave Select Polarity (SS is active low)
    SPI3CONbits.MSSEN = 1;      // Slave Select Enable (SS driven during transmission)
    SPI3CONbits.FRMSYPW = 1;    // Frame Sync Pulse Width (frame sync pulse is one clock wide)
    SPI3CONbits.FRMCNT = 0b101; // Frame Sync Pulse Counter (pulse on every data character)
    SPI3CONbits.SPIFE = 0;      // Frame Sync Pulse Edge Select (frame sync precedes first bit)
    SPI3CONbits.ENHBUF = 0;     // Enhanced Buffer Enable (disable enhanced buffer)
    SPI3CONbits.SIDL = 1;       // Stop in Idle Mode
    SPI3CONbits.DISSDO = 0;     // Disable SDOx (pin is controlled by this module)
    SPI3CONbits.MODE32 = 0;     // Do not use 32-bit mode (use 8-bit mode)
    SPI3CONbits.MODE16 = 0;     // Do not use 16-bit mode (use 8-bit mode)
    SPI3CONbits.SMP = 0;        // Input data is sampled at the end of the clock signal
    SPI3CONbits.CKE = 1;        // Data is shifted out/in on transition from idle state to active state
    SPI3CONbits.SSEN = 1;       // Slave Select Enable (SS pin used by module)
    SPI3CONbits.CKP = 1;        // Clock Polarity Select (clock signal is active high, idle state is low)
    SPI3CONbits.MSTEN = 1;      // Master Mode Enable
    SPI3CONbits.STXISEL = 0b01; // SPI Transmit Buffer Empty Interrupt Mode (generated when the buffer is completely empty)
    SPI3CONbits.SRXISEL = 0b11; // SPI Receive Buffer Full Interrupt Mode (generated when the buffer is full)
    
    // SCLK period > 70ns
    // 70ns ~= 14.3MHz
    // F_SCK = 14MHz
    
    // BRG = (F_PB / 2 * F_SCK) - 1
    // BRG = 1.86
    // BRG >= 2

    SPI3BRG = 20;
    SPI3CONbits.ON = 1;         // Configuration is done, turn on SPI3 peripheral
}


void ADS1298R_init() {
    ADS1298R_GPIO_init();
    ADS1298R_SPI_init();

    // Set CLKSEL pin = 1
    PORTDbits.RD8 = 1;
    
    // Set nPDWN = 1
    /* HARDWARE SET USING SW2 */

    // Set nRESET = 1
    /* HARDWARE SET USING SW3 */

    // Wait for t_POR for Power-On Reset
    
    // Issue Reset Pulse
    /* MUST BE DONE USING SW3... */

    // Wait for 18 t_CLK
    
    // Send SDATAC command
    ADS1298R_write(SDATAC);
    
    // SET PDB_REFBUF = 1
        // WREG CONFIG3 0xC0
    write_register(CONFIG3, 0xC0);
    
    // Write Certain Registers
        // WREG CONFIG1 0x86
    write_register(CONFIG1, 0x86);
        // WREG CONFIG2 0x00
    write_register(CONFIG2, 0x00);
    // Set all channels to input short
        // WREG CHnSET 0x01
    write_register(CH1SET, 0x01);
    write_register(CH2SET, 0x01);
    write_register(CH3SET, 0x01);
    write_register(CH4SET, 0x01);
    write_register(CH5SET, 0x01);
    write_register(CH6SET, 0x01);
    write_register(CH7SET, 0x01);
    write_register(CH8SET, 0x01);
    
    // Set START = 1
    PORTDbits.RD10 = 1;
    
    // RDATAC
    ADS1298R_write(RDATAC);
/*
    // Set Test Signals
        // SDATAC
    ADS1298R_write(RDATAC);
        // WREG CONFIG2 0x10
    write_register(CONFIG2, 0x10);
        // WREG CHnSET 0x05
    write_register(CH1SET, 0x05);
    write_register(CH2SET, 0x05);
    write_register(CH3SET, 0x05);
    write_register(CH4SET, 0x05);
    write_register(CH5SET, 0x05);
    write_register(CH6SET, 0x05);
    write_register(CH7SET, 0x05);
    write_register(CH8SET, 0x05);
        // RDATAC
    ADS1298R_write(RDATAC);
    
    // Capture Data and Test Signal
        // Look for nDRDY and issue 24 + n x 24 SCLKs
*/
}

uint8_t read_data() {
    return ADS1298R_write(0);
}

uint8_t data_ready() {
    return PORTDbits.RD7 == 0;
}

#endif /* _ADS1298R_H_ */