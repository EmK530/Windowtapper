#include "window_manager.h"
#include "window_motion.h"
#include "essentials.h"
#include "patterns.h"
#include <windows.h>
#include <stdint.h>

#define BG_R 16
#define BG_G 22
#define BG_B 104

#define DR_R 59
#define DR_G 70
#define DR_B 255

#define MAX_FRAMES 5
#define GRID_SIZE 11
#define SQUARE_SIZE 20

static WindowMotion motion;

static uint16_t current_pattern[GRID_SIZE];
static int pattern_frames = 0;

static uint16_t patterns[20][GRID_SIZE];

static void win_init(int id)
{
    memcpy(patterns, dec_patterns + 29, sizeof(patterns));

    mov_init(&motion, windows[id].hwnd);
    pattern_frames = 0;
    for(int i=0;i<GRID_SIZE;i++) current_pattern[i]=0;
}

static int pattern_id = -1;
static double pattern_dt = 0.0;
static int flip = 0;
static void win_event(int id,int sample)
{
    int copy_id = -1;
    int move_id = -1;

    if(sample >= 24 && sample <= 31)
    {
        copy_id = sample-24; move_id = copy_id%2;
    } else if(sample >= 41 && sample <= 44)
    {
        copy_id = sample-33; move_id = copy_id%2;
    } else if(sample >= 53 && sample <= 56)
    {
        copy_id = sample-41; move_id = copy_id%2;
    } else if(sample == 130) {
        copy_id = 16;
        flip = 1;
    } else if(sample == 129) {
        copy_id = 19;
        flip = 0;
    }

    if(move_id != -1)
    {
        switch(move_id)
        {
            case 0:
                mov_bump(&motion, -motion.height / 16, 0);
                break;
            case 1:
                mov_bump(&motion, motion.height / 16, 0);
                break;
        }
    }

    if(copy_id != -1)
    {
        memcpy(current_pattern, patterns[copy_id], sizeof(current_pattern));
        pattern_frames = MAX_FRAMES;
        pattern_id = copy_id;
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
    int blank = (pattern_id < 16 && pattern_frames == 0);

    if(pattern_id >= 16)
    {
        if(flip)
        {
            pattern_id = 16 + (int)(pattern_dt*1.8);
            if(pattern_id > 19)
                pattern_id = 19;
        } else {
            pattern_id = 19 - (int)(pattern_dt*1.8);
        }
    }

    // Skip drawing if nothing changed
    if(blank == last_blank && pattern_id == last_pattern_id)
        return;

    if(pattern_id >= 16)
        memcpy(current_pattern, patterns[pattern_id], sizeof(current_pattern));

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

WindowModule Noise_module = {
    "Noise",
    win_init,
    win_event,
    win_update,
    win_draw
};