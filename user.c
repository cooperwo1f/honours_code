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

ChannelData ch;

void init() {
//    ADC_init();
    ESP32_init();
    ADS1294R_init();
}

void run() {
    if (data_ready()) {
        read_data(&ch);
        debug(
            "HEADER: 0x%06X \n"
            "CH1: %u \n"
            "CH2: %u \n"
            "CH3: %u \n"
            "CH4: %u \n",
            ch.HEAD, ch.CH1, ch.CH2, ch.CH3, ch.CH4
        );
    }
    delay(500);
}
