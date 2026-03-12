#include <windows.h>
#include <stdint.h>
#include "essentials.h"
#include "window_manager.h"

WindowInstance windows[MAX_WINDOWS];
int window_count = 0;

extern WindowModule* modules[];

static HINSTANCE g_instance;

static LRESULT CALLBACK wm_proc(HWND h, UINT m, WPARAM w, LPARAM l)
{
    if(m == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(h,m,w,l);
}

void wm_init(HINSTANCE instance)
{
    g_instance = instance;

    WNDCLASSA wc;
    memset(&wc,0,sizeof(wc));

    wc.lpfnWndProc = wm_proc;
    wc.hInstance = instance;
    wc.lpszClassName = "ws";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

    RegisterClassA(&wc);
}

int wm_create(int module,int x,int y,int w,int h)
{
    if(window_count >= MAX_WINDOWS)
        return -1;

    h += 20;
    HWND hwnd = CreateWindowExA(
        0,
        "ws",
        modules[module]->title,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        x,y,w,h,
        0,0,
        g_instance,
        0
    );

    if(!hwnd)
        return -1;

    int id = window_count++;

    windows[id].hwnd = hwnd;
    windows[id].dc = GetDC(hwnd);
    windows[id].module = module;
    windows[id].x = x;
    windows[id].y = y;
    windows[id].w = w;
    windows[id].h = h;

    if(modules[module]->init)
        modules[module]->init(id);

    return id;
}

void wm_dispatch_sample(int sample)
{
    for(int i=0;i<window_count;i++)
    {
        WindowModule* m = modules[windows[i].module];
        if(m->event)
            m->event(i,sample);
    }
}

void wm_update(double d)
{
    for(int i=0;i<window_count;i++)
    {
        WindowModule* m = modules[windows[i].module];
        if(m->update)
            m->update(i, d);
    }
}

void wm_draw()
{
    for(int i=0;i<window_count;i++)
    {
        HWND hwnd = windows[i].hwnd;

        WindowModule* m = modules[windows[i].module];
        if(m->draw)
            m->draw(i,windows[i].dc);
    }
}

void wm_pump()
{
    MSG msg;

    while(PeekMessageA(&msg,0,0,0,PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

void wm_draw_bitpattern(int id, const uint16_t pattern[], int grid_size, COLORREF fg, COLORREF bg)
{
    RECT r;
    GetClientRect(windows[id].hwnd, &r);
    int client_w = r.right - r.left;
    int client_h = r.bottom - r.top;

    HDC dc = windows[id].dc;
    HBRUSH hbr_bg = CreateSolidBrush(bg);
    FillRect(dc, &r, hbr_bg);
    DeleteObject(hbr_bg);

    if(!pattern) { return; } //ReleaseDC(windows[id].hwnd, dc); return; }

    int square_w = client_w / grid_size;
    int square_h = client_h / grid_size;

    HBRUSH hbr_fg = CreateSolidBrush(fg);
    for(int y=0; y<grid_size; y++)
    {
        uint16_t row = pattern[y];
        for(int x=0; x<grid_size; x++)
        {
            if(row & (1<<(grid_size-1-x)))
            {
                RECT sq = {
                    x*square_w,
                    y*square_h,
                    (x+1)*square_w,
                    (y+1)*square_h
                };
                FillRect(dc, &sq, hbr_fg);
            }
        }
    }
    DeleteObject(hbr_fg);
    //ReleaseDC(windows[id].hwnd, dc);
}