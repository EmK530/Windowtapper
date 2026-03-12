#pragma once

#include <stdint.h>
#include <windows.h>

#define HEADER_SIZE 136 // We know it's 136 bytes

void* wmalloc(uint32_t size);
void wfree(void* ptr);
const uint8_t* load_resource(int id);
void print(const char* str);
void print_uint(uint32_t v);

typedef struct {
    const uint8_t* data;
    uint32_t size;
    uint32_t byte_pos;
    uint8_t bit_pos;
} BitReader;

void BitReader_Init(BitReader* br, const uint8_t* data, uint32_t size);
uint32_t BitReader_ReadBits(BitReader* br, uint8_t n);
uint32_t BitReader_ReadVarLen(BitReader* br);

static inline double get_time_sec() {
    return (double)GetTickCount64()/1000.0;
}
static inline uint32_t BitReader_BytesConsumed(BitReader* br) {
    return br->byte_pos + !!br->bit_pos;
}