#include "window_manager.h"
#include "window_motion.h"
#include "essentials.h"
#include "patterns.h"
#include <windows.h>
#include <stdint.h>

#define BG_R 108
#define BG_G 27
#define BG_B 50

#define DR_R 255
#define DR_G 84
#define DR_B 132

#define MAX_FRAMES 3
#define GRID_SIZE 11
#define SQUARE_SIZE 20

static WindowMotion motion;

static uint16_t current_pattern[GRID_SIZE];
static int pattern_frames = 0;

static uint16_t patterns[20][GRID_SIZE];

static void win_init(int id)
{
    memcpy(patterns, dec_patterns + 58, sizeof(patterns));

    mov_init(&motion, windows[id].hwnd);
    pattern_frames = 0;
    for (int i = 0; i < GRID_SIZE; i++)
        current_pattern[i] = 0;
}

static uint8_t pattern_map[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,2,3,1,2,3,4,1,5,5,5,5,6,4,7,5,8,9,5,8,9,10,10,10,10,10,1,11,
    2,7,7,7,12,12,12,10,10,10,10,13,6,6,0,14,14,2,2,2,2,15,0,15,3,3,16,16,2,17,17,2,3,13,3,18,18,18,
    7,7,7,7,0,0,0,0,12,12,10,10,6,9,15,18,10,10,18,4,5,5,2,1,17,5,5,14,11,3,3,3,18,18,18,18,18,9,
    7,7,7,7,12,12,15,12,12,15,12,12,0,9,6,6,6,6,8,8,5,8,10,10,4,16,10,10,4,13,17,13,0,14,14,3,13,13,
    19,19,19,19,19,19,19,19,19,19,19,19
};

static int pattern_id = -1;
static void win_event(int id, int sample)
{
    int copy_id = -1;

    if (sample >= 137 && sample <= 304)
    {
        copy_id = pattern_map[sample-137];
        switch(copy_id%4)
        {
            case 0:
                mov_bump(&motion, 0, -motion.height / 10);
                break;
            case 1:
                mov_bump(&motion, -motion.height / 10, 0);
                break;
            case 2:
                mov_bump(&motion, 0, motion.height / 10);
                break;
            case 3:
                mov_bump(&motion, motion.height / 10, 0);
                break;
        }
    }

    if (copy_id != -1)
    {
        memcpy(current_pattern, patterns[copy_id], sizeof(current_pattern));
        pattern_frames = MAX_FRAMES;
        pattern_id = copy_id;
    }
}

static void win_update(int id, double dt)
{
    if (pattern_frames > 0)
        pattern_frames--;

    mov_update(&motion, windows[id].hwnd, 40, dt);
}

static int last_blank = -1;
static int last_pattern_id = -1;

static void win_draw(int id, HDC dc)
{
    int blank = (pattern_frames == 0);

    // Skip drawing if nothing changed
    if (blank == last_blank && pattern_id == last_pattern_id)
        return;

    last_blank = blank;
    last_pattern_id = pattern_id;

    if (blank)
    {
        wm_draw_bitpattern(id, NULL, GRID_SIZE, 0, RGB(BG_R, BG_G, BG_B));
    }
    else
    {
        wm_draw_bitpattern(id, current_pattern, GRID_SIZE, RGB(DR_R, DR_G, DR_B), RGB(BG_R, BG_G, BG_B));
    }
}

WindowModule Radiolab_module = {
    "Radiolab",
    win_init,
    win_event,
    win_update,
    win_draw};