#include "window_manager.h"
#include "window_motion.h"
#include "essentials.h"
#include "patterns.h"
#include <windows.h>
#include <stdint.h>

#define BG_R 107
#define BG_G 53
#define BG_B 16

#define DR_R 252
#define DR_G 144
#define DR_B 58

#define MAX_FRAMES 10
#define GRID_SIZE 11
#define SQUARE_SIZE 20

static WindowMotion motion;

static uint16_t current_pattern[GRID_SIZE];
static int pattern_frames = 0;

static uint16_t patterns[4][GRID_SIZE];

static uint8_t pattern_map[64] = {
    255,0,255,0,255,0,255,0,255,1,
    255,1,255,1,255,1,255,2,255,2,
    255,2,255,2,255,2,255,2,255,3,
    255,3,255,0,255,0,255,0,255,0,
    255,1,255,1,255,1,255,1,255,2,
    255,2,255,2,255,2,255,2,255,2,
    255,3,255,3
};

static void win_init(int id)
{
    memcpy(patterns, dec_patterns + 92, sizeof(patterns));

    mov_init(&motion, windows[id].hwnd);
    pattern_frames = 0;
    for(int i=0;i<GRID_SIZE;i++) current_pattern[i]=0;
}

static int pattern_id = -1;
static void win_event(int id,int sample)
{
    int copy_id = -1;

    if((sample >= 305 && sample <= 320) || (sample >= 57 && sample <= 72))
    {
        if(sample%2 == 0)
        {
            copy_id = 0;
        }
    } else if(sample >= 341 && sample <= 372)
    {
        copy_id = pattern_map[sample-341];
    } else if(sample >= 101 && sample <= 108)
    {
        copy_id = pattern_map[sample-69];
    } else if(sample >= 119 && sample <= 126)
    {
        copy_id = pattern_map[sample-79];
    } else if(sample >= 373 && sample <= 388)
    {
        copy_id = pattern_map[sample-325];
    }

    if(copy_id != -1 && copy_id != 255)
    {
        mov_bump(&motion, 0, -motion.height / 12);
        memcpy(current_pattern, patterns[copy_id], sizeof(current_pattern));
        pattern_frames = MAX_FRAMES;
        pattern_id = copy_id;
    }
}

static void win_update(int id, double dt)
{
    if(pattern_frames > 0)
        pattern_frames--;

    mov_update(&motion, windows[id].hwnd, 160, dt);
}

static int last_blank = -1;
static int last_pattern_id = -1;

static void win_draw(int id, HDC dc)
{
    int blank = (pattern_frames == 0);

    // Skip drawing if nothing changed
    if(blank == last_blank && pattern_id == last_pattern_id)
        return;

    last_blank = blank;
    last_pattern_id = pattern_id;

    if(blank) {
        wm_draw_bitpattern(id, NULL, GRID_SIZE, 0, RGB(BG_R,BG_G,BG_B));
    } else {
        wm_draw_bitpattern(id, current_pattern, GRID_SIZE,
            RGB(DR_R,DR_G,DR_B),
            RGB(BG_R,BG_G,BG_B)
        );
    }
}

WindowModule Chord_module = {
    "Chord",
    win_init,
    win_event,
    win_update,
    win_draw
};