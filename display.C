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

void InitializeDisplay(const std::string &banner) {
  lcd_touch_startup();
  clear_screen();
  //put_up_grid();
}

void DrawDelay() {
}

void UpdateDisplay() {
}

void DrawLine(int p0x, int p0y, int p1x, int p1y, int draw_color,int draw_style) {
  draw_line(p0x,p0y,p1x,p1y,draw_color,draw_style);
}

void DrawDot(int px, int py, unsigned R, unsigned G, unsigned B) {
  //if ( (px < 0) || (px > 1024) || (py < 0) || (py > 1024) )
  //  return;
  //R = R % 256;
  unsigned int point_color = (R<<11) | (G<<5) | B; // 5 bits per color
  draw_point(py,px,point_color);
}

void DrawText(int cx, int cy, const std::string ts) {
  for (auto i = 0; i < ts.size(); i++) {
     display_char(cy + i, cx, ts[i], FONT_SIZE_16, COLOR_BLACK, COLOR_WHITE);
  }
}

void CloseDisplay() {
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
  return WINDOW_Y_OFFSET + y; //(WINDOW_HEIGHT - y);
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

#define CROSSHAIR_SIZE 20
void display_crosshairs(int x, int y) {
  tigrLine(screen, x - CROSSHAIR_SIZE, y, x + CROSSHAIR_SIZE, y, scolor(WHITE));
  tigrLine(screen, x, y - CROSSHAIR_SIZE, x, y + CROSSHAIR_SIZE, scolor(WHITE));
}
void display_window_border(int ulX, int ulY, int lrX, int lrY) {
  tigrLine(screen,ulX,ulY,lrX,ulY,scolor(WHITE));
  tigrLine(screen,lrX,ulY,lrX,lrY,scolor(WHITE));
  tigrLine(screen,ulX,ulY,ulX,lrY,scolor(WHITE));
  tigrLine(screen,ulX,lrY,lrX,lrY,scolor(WHITE));
}
void InitializeDisplay(const std::string &banner) {
  screen = tigrWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello", 0);

  tigrClear(screen, scolor(BLACK) ); //tigrRGB(0x80, 0x90, 0xa0));
  tigrPrint(screen, tfont, 120, 50, tigrRGB(0xff, 0xff, 0xff), banner.c_str());
  put_up_grid();
}

void EraseDisplay() {
  tigrClear(screen, scolor(BLACK) ); //tigrRGB(0x80, 0x90, 0xa0));
}

void DrawLine(int p0x, int p0y, int p1x, int p1y, int draw_color,int draw_style) {
  tigrLine(screen, transposeX(p0x), transposeY(p0y), 
          transposeX(p1x), transposeY(p1y), scolor(draw_color,draw_style));
}

void DrawDot(int px, int py, unsigned R, unsigned G, unsigned B) {
  //if ( (px < 0) || (px > 1024) || (py < 0) || (py > 1024) )
  //  return;
  R = R % 256;
  //px += 1024;
  //std::cout << "x/y: " << px << "/" << py << " R/G/B: " << std::hex 
  //<< R << "/" << G << "/" << B << std::dec << std::endl;

  //TPixel tc = (R == G == B == 0) ? scolor(WHITE) : scolor(GREEN);
  TPixel tc = tigrRGB(R,G,B);
  tigrPlot(screen, px, py, tc );
}

void DrawText(int cx, int cy,const std::string ts) {

}

void UpdateDisplay() {
    tigrUpdate(screen);
    //put_up_grid();
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

void getPointerState(int *x, int *y, int *buttons) {
  tigrMouse(screen, x, y, buttons);
}

int getDisplayChar() {
  return tigrReadChar(screen);
}
#endif
