#pragma once
#include <windows.h>

typedef struct {
    double target_x;
    double target_y;
    double cur_x;
    double cur_y;
    int height;
} WindowMotion;

void mov_init(WindowMotion* m, HWND hwnd);
void mov_bump(WindowMotion* m, double dx, double dy);
void mov_update(WindowMotion* m, HWND hwnd, int return_speed_div, double dt);
int mov_at_target(WindowMotion* m);