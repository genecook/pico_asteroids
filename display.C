#include <display.h>

#ifdef PICO_DISPLAY
extern "C" {
#include <lcd_touch_wrapper.h>
}

void put_up_grid() {
  // not a grid but window border...
  DrawLine(10, 10, WINDOW_WIDTH - 1, 10, RED,LINE_SOLID);
  DrawLine(WINDOW_WIDTH - 1, 10, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1, RED,LINE_SOLID);
  DrawLine(WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1, 10, WINDOW_HEIGHT - 1, RED,LINE_SOLID);
  DrawLine(0, WINDOW_HEIGHT - 1, 10,10, RED,LINE_SOLID);
}

void InitializeDisplay() {
  lcd_touch_startup();
  clear_screen();
  put_up_grid();
}

void DrawDelay() {
}

void UpdateDisplay() {
}

void DrawLine(int p0x, int p0y, int p1x, int p1y, int draw_color,int draw_style) {
  draw_line(p0x,p0y,p1x,p1y,draw_color,draw_style);
}

#else
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <time.h>
#include <unistd.h>

#include <tigr.h>

#define WHITE 0
#define BLACK 1
#define RED   2
#define GREEN 3
#define BLUE  4
#define GREY  5

int transposeX(int x) {
  return WINDOW_X_OFFSET + x;
}
int transposeY(int y) {
  return WINDOW_Y_OFFSET + (WINDOW_HEIGHT - y);
}

TPixel scolor(int wcolor, int wstyle = 0) {
  TPixel rc;
  switch(wcolor) {
    case WHITE: rc = tigrRGB(0xff, 0xff, 0xff); break;
    case BLACK: rc = tigrRGB(0, 0, 0); break;
    case RED:   rc = tigrRGB(0xff, 0, 0); break;
    case GREEN: rc = tigrRGB(0, 0xff, 0); break;
    case BLUE:  rc = tigrRGB(0, 0, 0xff); break;
    case GREY:  rc = tigrRGB(0x7f, 0x7f, 0x7f); break;
    default: break;
  }
  return rc;
}

Tigr *screen = NULL;

void put_up_grid() {
  tigrLine(screen, 0, WINDOW_ULY, SCREEN_WIDTH - 1, WINDOW_ULY, scolor(GREY));
  tigrLine(screen, 0, WINDOW_LRY, SCREEN_WIDTH - 1, WINDOW_LRY, scolor(GREY));

  tigrLine(screen, WINDOW_ULX, 0, WINDOW_ULX, SCREEN_HEIGHT - 1, scolor(GREY));
  tigrLine(screen, WINDOW_LRX, 0, WINDOW_LRX, SCREEN_HEIGHT - 1, scolor(GREY));
}

void InitializeDisplay() {
  screen = tigrWindow(1024,1024,"Hello", 0);
  tigrClear(screen, scolor(BLACK) ); //tigrRGB(0x80, 0x90, 0xa0));
  tigrPrint(screen, tfont, 120, 110, tigrRGB(0xff, 0xff, 0xff), "Hello, world.");
  put_up_grid();
}

void DrawLine(int p0x, int p0y, int p1x, int p1y, int draw_color,int draw_style) {
  tigrLine(screen, transposeX(p0x), transposeY(p0y), 
          transposeX(p1x), transposeY(p1y), scolor(draw_color,draw_style));
}

void UpdateDisplay() {
    tigrUpdate(screen);
    put_up_grid();
}

void DrawDelay() {
    usleep(10000);
}

void CloseDisplay() {
      while (!tigrClosed(screen)) {
        tigrUpdate(screen);
  }

  tigrFree(screen);
}
#endif
