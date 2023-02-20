#include <stdio.h>
#include <iostream>
#include <string>
#include <time.h>
#include <unistd.h>

#include <ast_obj.h>
#include <tigr.h>

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

#define WHITE 0
#define BLACK 1
#define RED   2
#define GREEN 3
#define BLUE  4
#define GREY  5

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

int transposeX(int x) {
  return WINDOW_X_OFFSET + x;
}
int transposeY(int y) {
  return WINDOW_Y_OFFSET + (WINDOW_HEIGHT - y);
}

void DrawLine(int p0x, int p0y, int p1x, int p1y, int draw_color,int draw_style) {
  tigrLine(screen, transposeX(p0x), transposeY(p0y), 
          transposeX(p1x), transposeY(p1y), scolor(draw_color,draw_style));
}

int main() {
  std::cout << "Test astObj class..." << std::endl;

  screen = tigrWindow(1024,1024,"Hello", 0);

  std::vector<struct coordinate> my_outline;

  // this square is all in...

  my_outline.push_back(coordinate(0,0));
  my_outline.push_back(coordinate(100,0));
  my_outline.push_back(coordinate(100,100));
  my_outline.push_back(coordinate(0,100));
  my_outline.push_back(coordinate(0,0));

  std::cout << "On screen instance. move it a few times within screen..." << std::endl;
  class astObj my_astobj(&my_outline,-50,270);

  my_astobj.SetTrajectory(10, -10);

  tigrClear(screen, scolor(BLACK) ); //tigrRGB(0x80, 0x90, 0xa0));
  tigrPrint(screen, tfont, 120, 110, tigrRGB(0xff, 0xff, 0xff), "Hello, world.");
  put_up_grid();

  //struct timespec st;
  //clock_gettime(CLOCK_MONOTONIC, &st);

  for(auto i = 0; i < 40; i++) {
    put_up_grid();
    my_astobj.Advance();
    tigrUpdate(screen);

    sleep(1);
    //struct timespec se;
    //while (st.tv_sec <= se.tv_sec) {
    //  clock_gettime(CLOCK_MONOTONIC, &se);
    //}
    //std::cout << "<<<" << se.tv_sec << ">>>" << std::endl;
  }

  while (!tigrClosed(screen)) {
        tigrUpdate(screen);
  }

  tigrFree(screen);

  return 0;
  my_astobj.DumpLineSegments();
  std::cout << std::endl;

  my_astobj.Advance();
  my_astobj.DumpLineSegments();
  std::cout << std::endl;

  my_astobj.Advance();
  my_astobj.DumpLineSegments();
  std::cout << std::endl;
  
  std::cout << "Off screen instance. better not show up..." << std::endl;

  class astObj my_astobj2(&my_outline,-100,-100);
  my_astobj2.Advance();
  my_astobj2.DumpLineSegments();
  std::cout << std::endl;

  std::cout << "Now set trajectory to advance this hidden instance onto screen..." << std::endl;

  my_astobj2.SetTrajectory(100, 100);
  my_astobj2.Advance();
  my_astobj2.DumpLineSegments();
  std::cout << std::endl;

  std::cout << "Now set trajectory to cause object to move left, partially off screen..." << std::endl;

  my_astobj2.SetTrajectory(-10, 0);
  my_astobj2.Advance();
  my_astobj2.DumpLineSegments();
  std::cout << std::endl;

  return 0;
}

