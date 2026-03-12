#include "window_manager.h"
#include "window_motion.h"
#include "essentials.h"
#include "patterns.h"
#include <windows.h>
#include <stdint.h>

#define BG_R 53
#define BG_G 25
#define BG_B 104

#define DR_R 137
#define DR_G 81
#define DR_B 251

#define MIN_FRAMES 5
#define GRID_SIZE 11
#define SQUARE_SIZE 20

static WindowMotion motion;

static uint16_t current_pattern[GRID_SIZE];
static double pattern_frames = 0;

static uint16_t patterns[13][GRID_SIZE];

static void win_init(int id)
{
    memcpy(patterns, dec_patterns + 12, sizeof(patterns));
    mov_init(&motion, windows[id].hwnd);
    pattern_frames = 0;
    for(int i=0;i<GRID_SIZE;i++) current_pattern[i]=0;
}

static int pattern_id = -1;
static double pattern_dt = 0.0;

static void win_event(int id,int sample)
{
    int copy_id = -1;
    int target_frames = MIN_FRAMES;

    if(sample == 93)
    {
        copy_id = 0; mov_bump(&motion, 0, motion.height / 10); target_frames = 9600;
    } else if(sample == 134 || sample == 135)
    {
        copy_id = 1; mov_bump(&motion, motion.height / 10, 0); target_frames = 9600;
    } else if(sample == 34 || sample == 94)
    {
        copy_id = 2; mov_bump(&motion, 0, -motion.height / 10); target_frames = sample == 94 ? 9600 : 15;
    } else if(sample == 51) {
        copy_id = 4; mov_bump(&motion, 0, -motion.height / 12);
    } else if(sample == 33) {
        copy_id = 5; mov_bump(&motion, motion.height / 14, -motion.height / 14);
    } else if(sample == 45) {
        copy_id = 6; mov_bump(&motion, -motion.height / 12, 0);
    } else if(sample == 136 || sample == 131) {
        copy_id = sample == 131 ? 3 : 7;
        target_frames = MIN_FRAMES;
    } else if(sample == 100 || sample == 133) {
        copy_id = 11;
        target_frames = 6;
    } else if(sample == 132) {
        copy_id = 12;
        target_frames = 16;
    } else if(sample == 46) {
        copy_id = 8; mov_bump(&motion, motion.height / 12, 0);
    } else if(sample == 32) {
        copy_id = 9; mov_bump(&motion, -motion.height / 14, motion.height / 14);
    } else if(sample == 52) {
        copy_id = 10; mov_bump(&motion, 0, motion.height / 12);
    }

    if(copy_id != -1)
    {
        pattern_id = copy_id;
        memcpy(current_pattern, patterns[copy_id], sizeof(current_pattern));
        pattern_frames = target_frames;
        pattern_dt = 0.0;
    }
}

static void win_update(int id, double dt)
{
    if(pattern_frames > 0)
        pattern_frames--;

    pattern_dt += dt;

    mov_update(&motion, windows[id].hwnd, 80, dt);
}

static int last_blank = -1;
static int last_pattern_id = -1;

static void win_draw(int id, HDC dc)
{
    int blank = (pattern_id == 3 ? (pattern_dt < 1.0 || pattern_dt > 2.25) : (pattern_frames == 0));

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

WindowModule DTMF_module = {
    "DTMF",
    win_init,
    win_event,
    win_update,
    win_draw
};