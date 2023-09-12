#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "user.h"
#include "util.h"
#include "ESP32.h"
#include "ADS1294R.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')

//struct {
//    uint32_t ECG:16;
//    uint32_t RSP:16;
//    uint32_t EMG:16;
//    uint32_t BPM:16;
//} packet;

void init() {
    ESP32_init();
    ADS1294R_init();
}

uint32_t config_bytes[BYTES_PER_CHANNEL];
uint32_t data[BYTES_TO_READ];

void run() {
    TP7_PIN = data_ready();
    
    if (data_ready()) {
        for (uint8_t i = 0; i < BYTES_PER_CHANNEL; i++) { config_bytes[i] = read_data(); }
        for (uint8_t i = 0; i < BYTES_TO_READ; i++) { data[i] = read_data(); }
        
        debug(  " config[0] = "BYTE_TO_BINARY_PATTERN
                " \n config[1] = "BYTE_TO_BINARY_PATTERN
                " \n config[2] = "BYTE_TO_BINARY_PATTERN, 
                BYTE_TO_BINARY(config_bytes[0]), 
                BYTE_TO_BINARY(config_bytes[1]), 
                BYTE_TO_BINARY(config_bytes[2]));
        
        debug(" CHANNEL1[0] = %u \n CHANNEL1[1] = %u \n CHANNEL1[2] = %u", data[0], data[1], data[2]);
    }
}
