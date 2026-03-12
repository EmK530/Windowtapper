#include "window_motion.h"
#include <math.h>
#include <stdio.h>

void mov_init(WindowMotion* m, HWND hwnd)
{
    RECT r;
    GetWindowRect(hwnd, &r);
    m->cur_x = m->target_x = (double)r.left;
    m->cur_y = m->target_y = (double)r.top;
    m->height = r.bottom - r.top - 20; // adjust for title bar
}

void mov_bump(WindowMotion* m, double dx, double dy)
{
    m->cur_x += dx;
    m->cur_y += dy;
}

void mov_update(WindowMotion* m, HWND hwnd, int return_speed_div, double dt)
{
    double speed_per_sec = (double)m->height / return_speed_div;
    double speed = speed_per_sec * (dt * 60.0);

    double dx = m->target_x - m->cur_x;
    if(fabs(dx) > 0.5) {
        if(fabs(dx) < speed) m->cur_x = m->target_x;
        else m->cur_x += (dx > 0 ? speed : -speed);
    } else m->cur_x = m->target_x;

    double dy = m->target_y - m->cur_y;
    if(fabs(dy) > 0.5) {
        if(fabs(dy) < speed) m->cur_y = m->target_y;
        else m->cur_y += (dy > 0 ? speed : -speed);
    } else m->cur_y = m->target_y;

    int ix = (int)(m->cur_x + 0.5);
    int iy = (int)(m->cur_y + 0.5);

    RECT r;
    GetWindowRect(hwnd, &r);
    int wx = r.left, wy = r.top;

    if(ix != wx || iy != wy)
        SetWindowPos(hwnd, 0, ix, iy, 0, 0,
                     SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

int mov_at_target(WindowMotion* m)
{
    return (fabs(m->cur_x - m->target_x) < 0.5) &&
           (fabs(m->cur_y - m->target_y) < 0.5);
}