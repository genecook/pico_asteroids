#ifndef __DISPLAY__

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 1024 

#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH  480
#endif
#ifndef WINDOW_HEIGHT
#define WINDOW_HEIGHT 320
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

void InitializeDisplay();
void DrawLine(int p0x, int p0y, int p1x, int p1y, int draw_color, int draw_style);

void UpdateDisplay();
void DrawDelay();
void CloseDisplay();

#endif
#define __DISPLAY__
