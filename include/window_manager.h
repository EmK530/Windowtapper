#pragma once

#include <windows.h>
#include <stdint.h>

#define MAX_WINDOWS 16

typedef struct WindowModule {
    const char* title;
    void (*init)(int id);
    void (*event)(int id, int sample);
    void (*update)(int id, double dt);
    void (*draw)(int id, HDC dc);
} WindowModule;

typedef struct WindowInstance {
    HWND hwnd;
    HDC dc;
    int module;
    int x,y,w,h;
} WindowInstance;

extern WindowInstance windows[MAX_WINDOWS];
extern int window_count;

void wm_init(HINSTANCE instance);
int wm_create(int module,int x,int y,int w,int h);

void wm_dispatch_sample(int sample);
void wm_update(double dt);
void wm_draw();
void wm_pump();
void wm_draw_bitpattern(int id, const uint16_t pattern[], int grid_size, COLORREF fg, COLORREF bg);