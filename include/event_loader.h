#pragma once

#include <stdint.h>

#define TOTAL_GROUPS 1489
#define COMMON_DELTAS_LEN 16
#define MAX_SAMPLES 719
#define BMS_SIZE 8901 // We know it's 8901

typedef struct{
    uint32_t delta;
    uint8_t group_size;
    uint16_t* sid_list;
} Event;

Event* load_all_events();