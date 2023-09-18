#include <xc.h>
#include <stdint.h>
#include <string.h>

#include "user.h"
#include "util.h"
#include "ESP32.h"
#include "ADS1294R.h"

struct {
    uint32_t ECG:16;
    uint32_t RSP:16;
    uint32_t EMG:16;
    uint32_t BPM:16;
} packet;

void init() {
//    ADC_init();
    ESP32_init();
    ADS1294R_init();
}

void run() {
    uint8_t id = read_register(ID);
    debug("ID: %u \n", id);
}
