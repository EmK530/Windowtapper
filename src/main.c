#include <windows.h>
#include <stdint.h>

#include "audio_manager.h"
#include "essentials.h"
#include "event_loader.h"
#include "window_manager.h"
#include "patterns.h"

typedef struct {
    uint32_t event_tick;
    int module;
    int x, y;
} WindowSpawn;

// 1440p hardcoded, will be overwritten below
WindowSpawn spawn_list[] = {
    {1, 0, 680, 100},
    {30, 1, 980, 100},
    {120, 2, 1280, 100},
    {220, 3, 1580, 100},
    {280, 4, 680, 450},
    {380, 5, 980, 450},
    {480, 6, 1280, 450},
    {640, 7, 1580, 450},
    {890, 8, 680, 800},
    {905, 9, 980, 800},
    {910, 10, 1280, 800},
    {1890, 11, 1580, 800},
    {2985, 12, 680, 1150},
    {3145, 13, 980, 1150},
    {4700, 14, 1280, 1150}
};
// 2 next are after 910 at 910 as well
// #12 is at 1900

#define GRID_ROWS 4
#define GRID_COLS 4
#define GRID_MARGIN 30

static int win_size = 300;
void layout_windows(WindowSpawn* list, size_t count) {
    int screen_w  = GetSystemMetrics(SM_CXSCREEN);
    int screen_h  = GetSystemMetrics(SM_CYSCREEN);

    int max_width  = (screen_w  - (GRID_COLS + 1) * GRID_MARGIN) / GRID_COLS;
    int max_height = (screen_h - (GRID_ROWS + 1) * (GRID_MARGIN + 20) - 45) / GRID_ROWS; 
    win_size = max_width < max_height ? max_width : max_height;

    int grid_width  = GRID_COLS * win_size + (GRID_COLS - 1) * GRID_MARGIN;
    int grid_height = GRID_ROWS * win_size + (GRID_ROWS - 1) * (GRID_MARGIN + 20);

    int start_x = (screen_w - grid_width) / 2;
    int start_y = (screen_h - grid_height) / 2 - 45;

    for (size_t i = 0; i < count; i++) {
        int row = i / GRID_COLS;
        int col = i % GRID_COLS;

        list[i].x = start_x + col * (win_size + GRID_MARGIN);
        list[i].y = start_y + row * (win_size + GRID_MARGIN + 20);
    }
}

#define SPAWN_COUNT (sizeof(spawn_list)/sizeof(spawn_list[0]))
int next_spawn = 0;

#ifdef OPUS_BUILD
int main(){
#else
int WinMainCRTStartup(){
#endif
    if(audio_init(MAX_SAMPLES) != 0)
        return 1;

    Event* events = load_all_events();
    if(!events)
        return 1;

    const uint8_t* pattern_data = load_resource(2);
    decode_patterns_bin(pattern_data, 1656);

#ifdef OPUS_BUILD
    const uint8_t* audio_list = load_resource(1);
    const uint8_t* ptr = audio_list;
    print("Decompressing audio...\r\n\r\n");
#else
    HANDLE hFile = CreateFileA("samples.pck", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    if(hFile==INVALID_HANDLE_VALUE)
    {
        print("Could not find samples.pck");
        return 0;
    }
    DWORD fileSize = GetFileSize(hFile,NULL);
    uint8_t* buffer = wmalloc(fileSize);
    DWORD bytesRead;
    if(!ReadFile(hFile,buffer,fileSize,&bytesRead,NULL) || bytesRead!=fileSize) { CloseHandle(hFile); wfree(buffer); return 0; }
    CloseHandle(hFile);
    const uint8_t* ptr = buffer;
#endif
    for(uint32_t i = 0; i < MAX_SAMPLES; i++) {
        uint32_t size = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16);
        ptr += 3;

        memset(samples + i, 0, sizeof(SampleX2));

        load_sample_x2((char*)ptr, size, samples + i);

        ptr += size;
    }

    layout_windows(spawn_list, 15);

    wm_init(GetModuleHandleA(0));

    print("\nProgram ready, hide the console and enjoy the show.");

    double start = get_time_sec();
    double last_time = get_time_sec();
    uint32_t current_tick=0,event_count=1;

    MSG msg;

    for(uint32_t i=1; i<TOTAL_GROUPS; i++){
        Event event = events[i];

        current_tick += event.delta;
        while (1)
        {
            double now = get_time_sec();
            double elapsed = now - start;
            double dt = now - last_time;
            last_time = now;

            MSG msg;
            while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }

            wm_update(dt);
            wm_draw();
            wm_pump();

            double wait = current_tick / 120.0 - elapsed;
            if (wait <= 0)
                break;

            DWORD sleep_ms = (DWORD)(wait * 1000.0);
            if (sleep_ms > 8) sleep_ms = 8;
            Sleep(sleep_ms);
        }

        for(uint32_t j = 0; j < event.group_size; j++) {
            uint32_t sid = event.sid_list[j];
            event_count++;
            //print("Ev "); print_uint(event_count); print("/6007 - Sample "); print_uint(sid); print("\r\n");

            while(next_spawn < SPAWN_COUNT && event_count >= spawn_list[next_spawn].event_tick)
            {
                WindowSpawn ws = spawn_list[next_spawn];
                wm_create(ws.module, ws.x, ws.y, win_size, win_size);
                next_spawn++;
            }

            SampleX2* smp = samples + sid;
            if(smp->pcm_data && smp->pcm_size > 0 && smp->voice) {
                play_sample(smp);
            }
            wm_dispatch_sample(sid);
        }
    }

    double start2 = get_time_sec();
    while (get_time_sec()-start2<0.16)
    {
        double now = get_time_sec();
        double elapsed = now - start;
        double dt = now - last_time;
        last_time = now;

        MSG msg;
        while (PeekMessageA(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

        wm_update(dt);
        wm_draw();
        wm_pump();

        double wait = 0.008-dt;
        if(wait > 0)
        {
            DWORD sleep_ms = (DWORD)(wait * 1000.0);
            if (sleep_ms > 5) sleep_ms = 5;
            Sleep(sleep_ms);
        }
    }

    for(uint32_t i = 0; i < MAX_SAMPLES; i++) { free_sample(samples + i); }
    CoUninitialize();
    ExitProcess(0);
}