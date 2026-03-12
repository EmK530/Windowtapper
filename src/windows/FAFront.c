#include "window_manager.h"
#include "window_motion.h"
#include "essentials.h"
#include "patterns.h"
#include <windows.h>
#include <stdint.h>

#define BG_R 94
#define BG_G 75
#define BG_B 14

#define DR_R 243
#define DR_G 206
#define DR_B 55

#define MAX_FRAMES 5
#define GRID_SIZE 11
#define SQUARE_SIZE 20

static WindowMotion motion;

static uint16_t current_pattern[GRID_SIZE];
static int pattern_frames = 0;

static uint16_t patterns[3][GRID_SIZE];

static void win_init(int id)
{
    memcpy(patterns, dec_patterns + 26, sizeof(patterns));

    mov_init(&motion, windows[id].hwnd);
    pattern_frames = 0;
    for(int i=0;i<GRID_SIZE;i++) current_pattern[i]=0;
}

static int pattern_id = -1;
static void win_event(int id,int sample)
{
    int copy_id = -1;

    if(sample == 0)
    {
        copy_id = 0; mov_bump(&motion, 0, -motion.height / 12);
    } else if(sample == 14)
    {
        copy_id = 1; mov_bump(&motion, -motion.height / 12, 0);
    } else if(sample == 12)
    {
        copy_id = 2; mov_bump(&motion, motion.height / 12, 0);
    }

    if(copy_id != -1)
    {
        memcpy(current_pattern, patterns[copy_id], sizeof(current_pattern));
        pattern_frames = MAX_FRAMES;
        pattern_id = copy_id;
    }
}

static void win_update(int id, double dt)
{
    if(pattern_frames > 0)
        pattern_frames--;

    mov_update(&motion, windows[id].hwnd, 100, dt);
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

WindowModule FAFront_module = {
    "FA Front",
    win_init,
    win_event,
    win_update,
    win_draw
};