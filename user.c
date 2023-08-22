#include <stdint.h>
#include <string.h>

#include "user.h"
#include "util.h"
#include "ESP32.h"
#include "ADS1298R.h"

struct {
    uint32_t ECG:16;
    uint32_t RSP:16;
    uint32_t EMG:16;
    uint32_t BPM:16;
} packet;

void init() {
    ESP32_init();
    ADS1298R_init();
}

void run() {
    while (!data_ready()) {
        debug("%u", read_data());
    }
}
