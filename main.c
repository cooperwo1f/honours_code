
// PIC32MX775F512H Configuration Bit Settings

// 'C' source line config statements

// DEVCFG3
// USERID = No Setting
#pragma config FSRSSEL = PRIORITY_7     // SRS Select (SRS Priority 7)
#pragma config FMIIEN = OFF             // Ethernet RMII/MII Enable (RMII Enabled)
#pragma config FETHIO = OFF             // Ethernet I/O Pin Select (Alternate Ethernet I/O)
#pragma config FCANIO = OFF             // CAN I/O Pin Select (Alternate CAN I/O)
#pragma config FUSBIDIO = ON            // USB USID Selection (Controlled by the USB Module)
#pragma config FVBUSONIO = ON           // USB VBUS ON Selection (Controlled by USB Module)

// DEVCFG2
#pragma config FPLLIDIV = DIV_5         // PLL Input Divider (5x Divider)
#pragma config FPLLMUL = MUL_16         // PLL Multiplier (16x Multiplier)
#pragma config UPLLIDIV = DIV_6         // USB PLL Input Divider (6x Divider)
#pragma config UPLLEN = ON              // USB PLL Enable (Enabled)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)

// DEVCFG1
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = OFF               // Internal/External Switch Over (Disabled)
#pragma config POSCMOD = HS             // Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))

// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is disabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (ICE EMUC1/EMUD1 pins shared with PGC1/PGD1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "sine.h"

#define SYS_FREQ 80000000           // Crystal is 25MHz / 5 * 16 = 80MHz
#include "util.h"

typedef struct {
    uint8_t ECG;
    uint8_t RESP;
    uint8_t EMG;
    uint8_t BPM;
} Packet;

// http://www.ross.net/crc/download/crc_v3.txt
void SPI_packet(uint8_t *packet, uint16_t len) {
    uint8_t CRC = 0;
    
    for (uint16_t i = 0; i < len; i++) {
        SPI_write(packet[i]);
    }
    
    SPI_write(CRC);
}

void IO_init() {
    TRISBbits.TRISB2 = 0;       // Set ESP32 EN pin as output
    PORTBbits.RB2 = 1;          // Set ESP32 EN pin high
}

void SPI_init() {
    SPI2CONbits.ON = 0;         // Turn off SPI2 before configuring
    SPI2CONbits.FRMEN = 0;      // Framed SPI Support (SS pin used)
    SPI2CONbits.FRMSYNC = 0;    // Frame Sync Pulse Direction (pulse output, master mode)
    SPI2CONbits.FRMPOL = 0;     // Slave Select Polarity (SS is active low)
    SPI2CONbits.MSSEN = 1;      // Slave Select Enable (SS driven during transmission)
    SPI2CONbits.FRMSYPW = 0;    // Frame Sync Pulse Width (frame sync pulse is one clock wide)
    SPI2CONbits.FRMCNT = 0b000; // Frame Sync Pulse Counter (pulse on every data character)
    SPI2CONbits.SPIFE = 0;      // Frame Sync Pulse Edge Select (frame sync precedes first bit)
    SPI2CONbits.ENHBUF = 0;     // Enhanced Buffer Enable (disable enhanced buffer)
    SPI2CONbits.SIDL = 1;       // Stop in Idle Mode
    SPI2CONbits.DISSDO = 0;     // Disable SDOx (pin is controlled by this module)
    SPI2CONbits.MODE32 = 0;     // Use 32-bit mode
    SPI2CONbits.MODE16 = 0;     // Do not use 16-bit mode
    SPI2CONbits.SMP = 0;        // Input data is sampled at the end of the clock signal
    SPI2CONbits.CKE = 0;        // Data is shifted out/in on transition from idle (high) state to active (low) state
    SPI2CONbits.SSEN = 1;       // Slave Select Enable (SS pin used by module)
    SPI2CONbits.CKP = 1;        // Clock Polarity Select (clock signal is active low, idle state is high)
    SPI2CONbits.MSTEN = 1;      // Master Mode Enable
    SPI2CONbits.STXISEL = 0b01; // SPI Transmit Buffer Empty Interrupt Mode (generated when the buffer is completely empty)
    SPI2CONbits.SRXISEL = 0b11; // SPI Receive Buffer Full Interrupt Mode (generated when the buffer is full)
    SPI2BRG = 0;                // Set Baud Rate Generator to 0
    SPI2CONbits.ON = 1;         // Configuration is done, turn on SPI2 peripheral
}

void init() {
    IO_init();                  // turn on the ESP module EN
    SPI_init();                 // configure SPI2 to communicate with ESP
}

uint8_t i = 0;

void run() {
    SPI_write(sine_wave[i++]);
    delay_ms(20);
}

int main ( void ) {
    init();
    
    while (1) {
        run();
    }
    
}
