#include "window_manager.h"
#include "window_motion.h"
#include "essentials.h"
#include "patterns.h"
#include <windows.h>
#include <stdint.h>

#define BG_R 10
#define BG_G 78
#define BG_B 103

#define DR_R 52
#define DR_G 205
#define DR_B 248

#define MIN_FRAMES 4
#define GRID_SIZE 11
#define SQUARE_SIZE 20

static WindowMotion motion;

static uint16_t current_pattern[GRID_SIZE];
static int pattern_frames = 0;

static uint16_t patterns[9][GRID_SIZE];

static void win_init(int id)
{
    memcpy(patterns, dec_patterns + 49, sizeof(patterns));

    mov_init(&motion, windows[id].hwnd);
    pattern_frames = 0;
    for(int i=0;i<GRID_SIZE;i++) current_pattern[i]=0;
}

static int pattern_id = -1;
static void win_event(int id,int sample)
{
    int copy_id = -1;
    int target_frames = MIN_FRAMES;

    if(sample == 35)
    {
        copy_id = 0; mov_bump(&motion, motion.height / 12, 0);
    } else if(sample == 36) {
        copy_id = 1; mov_bump(&motion, 0, motion.height / 12);
    } else if(sample == 13) {
        copy_id = 2; mov_bump(&motion, -motion.height / 12, 0);
    } else if(sample == 10) {
        copy_id = 3; mov_bump(&motion, 0, -motion.height / 12);
    } else if(sample == 37) {
        copy_id = 4; mov_bump(&motion, -motion.height / 12, 0);
    } else if(sample == 38) {
        copy_id = 5; mov_bump(&motion, 0, -motion.height / 12);
    } else if(sample == 39) {
        copy_id = 6; mov_bump(&motion, motion.height / 12, 0);
    } else if(sample == 40) {
        copy_id = 7; mov_bump(&motion, 0, -motion.height / 12);
    } else if(sample == 23) {
        copy_id = 8; mov_bump(&motion, 0, motion.height / 12); target_frames = 6;
    } else if(sample == 19) {
        copy_id = 8; mov_bump(&motion, 0, motion.height / 12);
    }

    if(copy_id != -1)
    {
        memcpy(current_pattern, patterns[copy_id], sizeof(current_pattern));
        pattern_frames = target_frames;
        pattern_id = copy_id;
    }
}

static void win_update(int id, double dt)
{
    if(pattern_frames > 0)
        pattern_frames--;

    mov_update(&motion, windows[id].hwnd, 80, dt);
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

WindowModule PM_module = {
    "PM",
    win_init,
    win_event,
    win_update,
    win_draw
};