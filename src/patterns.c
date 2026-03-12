#include "patterns.h"
#include "essentials.h"
#include <stdint.h>
#include <string.h>

uint16_t dec_patterns[MAX_PATTERNS][GRID_SIZE];
uint16_t num_patterns = 0;

void decode_patterns_bin(const uint8_t* data, uint32_t size) {
    num_patterns = data[0] | (data[1] << 8);
    const uint8_t* ptr = data + 2;

    for (int p = 0; p < num_patterns; p++) {
        for (int y = 0; y < GRID_SIZE; y++) {
            uint16_t row = ptr[0] | (ptr[1] << 8);

            if (y != 0 && y != GRID_SIZE-1) row <<= 1;

            dec_patterns[p][y] = row;
            ptr += 2;
        }
    }
}