#include <stdint.h>
#include <string.h>

#include "user.h"
#include "util.h"
#include "ADS1298R.h"

struct {
    uint32_t ECG:16;
    uint32_t RSP:16;
    uint32_t EMG:16;
    uint32_t BPM:16;
} packet;

void init() {
    IO_init();
    ESP_init();
    ADS1298R_init();
}

void run() {
    uint8_t id = read_register(ID);
    debug("ID: %u \n", id);
    delay(500);
}
