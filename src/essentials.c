#include "essentials.h"

#include <windows.h>
#include <stdint.h>

void* wmalloc(uint32_t size)
{
    return HeapAlloc(GetProcessHeap(), 0, size);
}

void wfree(void* ptr)
{
    HeapFree(GetProcessHeap(), 0, ptr);
}

const uint8_t* load_resource(int id){
    HRSRC hRes = FindResourceA(NULL, MAKEINTRESOURCE(id), RT_RCDATA);
    if(!hRes) return NULL;
    HGLOBAL hMem = LoadResource(NULL,hRes);
    if(!hMem) return NULL;
    return LockResource(hMem);
}

void print(const char* str) {
    DWORD written;
    WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), str, lstrlenA(str), &written, NULL);
}

void print_uint(uint32_t v) {
    char buf[11];
    char *p = buf + 10;
    *p = 0;
    do {
        *--p = '0' + (v % 10);
        v /= 10;
    } while (v);
    print(p);
}

// BitReader

void BitReader_Init(BitReader* br, const uint8_t* data, uint32_t size) {
    *br = (BitReader){data, size, 0, 0};
}

uint32_t BitReader_ReadBits(BitReader* br, uint8_t n) {
    uint32_t v=0;
    while(n--){
        if(br->byte_pos>=br->size) return v;
        v=(v<<1)|((br->data[br->byte_pos]>>(7-br->bit_pos++))&1);
        if(br->bit_pos==8){ br->bit_pos=0; br->byte_pos++; }
    }
    return v;
}

uint32_t BitReader_ReadVarLen(BitReader* br){
    uint32_t v=0,s=0,b;
    do{
        b=BitReader_ReadBits(br,8);
        v|=(b&127)<<s;
        s+=7;
    }while(b&128);
    return v;
}