#include "event_loader.h"

#include "essentials.h"

#include <stdint.h>
#include <windows.h>

const uint8_t COMMON_DELTAS[COMMON_DELTAS_LEN]={32,9,4,5,3,21,33,48,6,2,22,64,65,0,49,0}; // Index 13 was over uint8_t limit so we hardcoded it

Event* load_all_events()
{
    const uint8_t* bms_data = load_resource(0);
    if(!bms_data) { return NULL; }

    BitReader br; BitReader_Init(&br,bms_data,BMS_SIZE);

    Event* events = wmalloc(TOTAL_GROUPS * sizeof(Event));

    for(uint32_t i=0;i<TOTAL_GROUPS;i++){
        uint32_t code = BitReader_ReadBits(&br,2);
        uint32_t delta;
        if(code==0) delta=8;
        else if(code==1) delta=16;
        else if(code==2) delta=17;
        else {
            uint32_t idx=BitReader_ReadBits(&br,4);
            if(idx==13) delta=257; else delta=COMMON_DELTAS[idx];
        }
        events[i].delta = delta;
    }

    uint32_t sample_data_start=BitReader_BytesConsumed(&br);
    BitReader sample_br; BitReader_Init(&sample_br,bms_data+sample_data_start,BMS_SIZE-sample_data_start);

    Event* cur = events;
    for(uint32_t i=0;i<TOTAL_GROUPS;i++){
        uint32_t num_samples = BitReader_ReadBits(&sample_br,4)+1;
        cur->group_size = num_samples;
        uint16_t* list = wmalloc(num_samples * sizeof(uint16_t));
        cur->sid_list = list;
        for(uint32_t j=0;j<num_samples;j++){
            (*list++) = BitReader_ReadVarLen(&sample_br);
        }
        cur++;
    }

    return events;
}