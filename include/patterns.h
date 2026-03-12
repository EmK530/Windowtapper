#pragma once
#include <stdint.h>

#define INNER_SIZE 9
#define GRID_SIZE 11
#define MAX_PATTERNS 96

extern uint16_t dec_patterns[MAX_PATTERNS][GRID_SIZE];
extern uint16_t num_patterns;

void decode_patterns_bin(const uint8_t* data, uint32_t size);