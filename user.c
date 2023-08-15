#include <string.h>

#include "user.h"
#include "util.h"

struct {
    uint32_t ECG:16;
    uint32_t RSP:16;
    uint32_t EMG:16;
    uint32_t BPM:16;
} packet;

void ADS1298R_init() {
    SPI3CONbits.ON = 0;         // Turn off SPI2 before configuring
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
    SPI3CONbits.MODE32 = 1;     // Use 32-bit mode
    SPI3CONbits.MODE16 = 0;     // Do not use 16-bit mode
    SPI3CONbits.SMP = 0;        // Input data is sampled at the end of the clock signal
    SPI3CONbits.CKE = 1;        // Data is shifted out/in on transition from idle (high) state to active (low) state
    SPI3CONbits.SSEN = 1;       // Slave Select Enable (SS pin used by module)
    SPI3CONbits.CKP = 0;        // Clock Polarity Select (clock signal is active low, idle state is high)
    SPI3CONbits.MSTEN = 1;      // Master Mode Enable
    SPI3CONbits.STXISEL = 0b01; // SPI Transmit Buffer Empty Interrupt Mode (generated when the buffer is completely empty)
    SPI3CONbits.SRXISEL = 0b11; // SPI Receive Buffer Full Interrupt Mode (generated when the buffer is full)
    
    // BRG = (F_PB / 2 * F_SCK) - 1
    // For F_SCK = 48000 = 48kHz
    // BRG = 832.333
    SPI3BRG = 832;
//    SPI2BRG = 50;

    SPI3CONbits.ON = 1;         // Configuration is done, turn on SPI2 peripheral
}

void init() {
    IO_init();
    ESP_init();
    //ADS1298R_init();
}

void run() {
    debug("Hello, World! \n");
}
