#pragma once

#include <xaudio2.h>
#include <stdint.h>

typedef struct {
    IXAudio2SourceVoice* voice;
    uint8_t* pcm_data;
    uint32_t pcm_size;
    uint8_t* full_buffer;
    WAVEFORMATEX format;
} SampleX2;

extern IXAudio2* g_xaudio;
extern IXAudio2MasteringVoice* g_master;
extern SampleX2* samples;

int audio_init(uint32_t sample_limit);
int load_sample_x2(char* file_data, uint32_t file_size, SampleX2* s);
void play_sample(SampleX2* s);
void free_sample(SampleX2* s);