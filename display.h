#ifndef __DISPLAY__

#include <string>

#ifdef PICO_DISPLAY
#include "LCD_Driver.h"
#include "LCD_Touch.h"
#include "LCD_GUI.h"
#include "LCD_Bmp.h"
#include "DEV_Config.h"
#endif

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 1024 

#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH  320
#endif
#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT 480
#endif

#define WINDOW_X_OFFSET (SCREEN_WIDTH - WINDOW_WIDTH) / 2
#define WINDOW_Y_OFFSET (SCREEN_HEIGHT - WINDOW_HEIGHT) / 2
#define WINDOW_ULX WINDOW_X_OFFSET
#define WINDOW_ULY WINDOW_Y_OFFSET + WINDOW_HEIGHT
#define WINDOW_LRX WINDOW_X_OFFSET + WINDOW_WIDTH
#define WINDOW_LRY WINDOW_Y_OFFSET

int transposeX(int x);
int transposeY(int y);

#define BOX_SIZE 100

void InitializeDisplay(const std::string &);

void DrawLine(int p0x, int p0y, int p1x, int p1y, int draw_color, int draw_style = 0);
void DrawDot(int px, int py, unsigned R, unsigned G, unsigned B);
void DrawText(int cx, int cy,const std::string ts);

void EraseDisplay();
void UpdateDisplay();
void DrawDelay();
void CloseDisplay();
void getPointerState(int *x, int *y, int *buttons);
int getDisplayChar();
void display_crosshairs(int x, int y);
void display_window_border(int ulX, int ulY, int lrX, int lrY);
#endif
#define __DISPLAY__
