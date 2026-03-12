#include "audio_manager.h"
#include "essentials.h"
#ifdef OPUS_BUILD
#include "opus/opus.h"
#include "ogg/ogg.h"
#endif

#include <xaudio2.h>

IXAudio2* g_xaudio = NULL;
IXAudio2MasteringVoice* g_master = NULL;
SampleX2* samples = NULL;

int audio_init(uint32_t sample_limit)
{
    if(FAILED(CoInitializeEx(NULL,COINIT_MULTITHREADED))){ return 1; }

    if(FAILED(XAudio2Create(&g_xaudio,0,XAUDIO2_DEFAULT_PROCESSOR))){ return 1; }
    if(FAILED(g_xaudio->lpVtbl->CreateMasteringVoice(g_xaudio,&g_master,XAUDIO2_DEFAULT_CHANNELS,XAUDIO2_DEFAULT_SAMPLERATE,0,NULL,NULL,AudioCategory_GameEffects))){ return 1; }

    samples = wmalloc(sample_limit * sizeof(SampleX2));

    return 0;
}

#ifndef OPUS_BUILD
int load_sample_x2(char* file_data, uint32_t file_size, SampleX2* s) {
    if(file_size<44){ return 0; }

    uint32_t data_offset = 44;
    uint32_t data_size = *(uint32_t*)(file_data+40);

    memcpy(&s->format,file_data+20,sizeof(WAVEFORMATEX));
    s->format.cbSize = 0;
    s->full_buffer = file_data;
    s->pcm_data = file_data + data_offset;
    s->pcm_size = data_size;

    if (FAILED(g_xaudio->lpVtbl->CreateSourceVoice(
        g_xaudio,
        &s->voice,
        &s->format,
        0,
        XAUDIO2_DEFAULT_FREQ_RATIO,
        NULL,
        NULL,
        NULL))) {
        return 0;
    }

    return 1;
}
#else
int load_sample_x2(char* file_data, uint32_t file_size, SampleX2* s)
{
    if(file_size < 4 ||
       file_data[0] != 'O' ||
       file_data[1] != 'g' ||
       file_data[2] != 'g' ||
       file_data[3] != 'S')
    {
        return 0;
    }

    ogg_sync_state oy;
    ogg_sync_init(&oy);

    char* ogg_buf = ogg_sync_buffer(&oy, file_size);
    memcpy(ogg_buf, file_data, file_size);
    ogg_sync_wrote(&oy, file_size);

    ogg_stream_state os;
    ogg_page og;
    ogg_packet op;

    if(ogg_sync_pageout(&oy, &og) != 1)
    {
        ogg_sync_clear(&oy);
        return 0;
    }

    ogg_stream_init(&os, ogg_page_serialno(&og));
    ogg_stream_pagein(&os, &og);

    int error;
    OpusDecoder* decoder = opus_decoder_create(48000, 2, &error);

    if(!decoder || error != OPUS_OK)
    {
        ogg_stream_clear(&os);
        ogg_sync_clear(&oy);
        return 0;
    }

    int max_samples = 48000 * 20;

    int16_t* pcm_buf = HeapAlloc(
        GetProcessHeap(),
        0,
        max_samples * 2 * sizeof(int16_t)
    );

    if(!pcm_buf)
    {
        opus_decoder_destroy(decoder);
        ogg_stream_clear(&os);
        ogg_sync_clear(&oy);
        return 0;
    }

    int total_samples = 0;

    int found_audio = 0;

    while(1)
    {
        if(ogg_stream_packetout(&os, &op) != 1)
        {
            if(ogg_sync_pageout(&oy, &og) != 1)
                break;

            ogg_stream_pagein(&os, &og);
            continue;
        }

        if(!found_audio)
        {
            if(op.bytes >= 8 && !memcmp(op.packet, "OpusHead", 8))
            {
                continue;
            }

            if(op.bytes >= 8 && !memcmp(op.packet, "OpusTags", 8))
            {
                continue;
            }

            found_audio = 1;
        }

        if(total_samples + 5760 >= max_samples)
        {
            break;
        }

        int frame_count = opus_decode(
            decoder,
            op.packet,
            op.bytes,
            pcm_buf + total_samples * 2,
            5760,
            0
        );

        if(frame_count < 0)
        {
            continue;
        }

        total_samples += frame_count;
    }

    if(total_samples == 0)
    {
        wfree(pcm_buf);
        opus_decoder_destroy(decoder);
        ogg_stream_clear(&os);
        ogg_sync_clear(&oy);
        return 0;
    }

    s->format.wFormatTag = WAVE_FORMAT_PCM;
    s->format.nChannels = 2;
    s->format.nSamplesPerSec = 48000;
    s->format.wBitsPerSample = 16;
    s->format.nBlockAlign = 4;
    s->format.nAvgBytesPerSec = 48000 * 4;
    s->format.cbSize = 0;

    s->full_buffer = (uint8_t*)pcm_buf;
    s->pcm_data = (uint8_t*)pcm_buf;
    s->pcm_size = total_samples * 4;

    if(FAILED(g_xaudio->lpVtbl->CreateSourceVoice(
        g_xaudio,
        &s->voice,
        &s->format,
        0,
        XAUDIO2_DEFAULT_FREQ_RATIO,
        NULL,
        NULL,
        NULL)))
    {

        wfree(pcm_buf);
        opus_decoder_destroy(decoder);
        ogg_stream_clear(&os);
        ogg_sync_clear(&oy);
        return 0;
    }

    opus_decoder_destroy(decoder);
    ogg_stream_clear(&os);
    ogg_sync_clear(&oy);

    return 1;
}
#endif

void play_sample(SampleX2* s) {
    XAUDIO2_BUFFER buf = {0};
    buf.AudioBytes = s->pcm_size;
    buf.pAudioData = s->pcm_data;
    buf.Flags = XAUDIO2_END_OF_STREAM;

    XAUDIO2_VOICE_STATE state;
    s->voice->lpVtbl->GetState(s->voice, &state, 0);
    if (state.BuffersQueued > 0) {
        s->voice->lpVtbl->Stop(s->voice, 0, XAUDIO2_COMMIT_NOW);
        s->voice->lpVtbl->FlushSourceBuffers(s->voice);
    }

    s->voice->lpVtbl->SubmitSourceBuffer(s->voice, &buf, NULL);
    s->voice->lpVtbl->Start(s->voice, 0, XAUDIO2_COMMIT_NOW);
}

void free_sample(SampleX2* s) {
    if (s->voice) s->voice->lpVtbl->DestroyVoice(s->voice);
    wfree(s->full_buffer);
}