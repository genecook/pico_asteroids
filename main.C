#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
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

#define BOX_SIZE 100

void StartCoordinates(int &x, int &y, int &start_square) {
  if (start_square < 0) {
    start_square = rand() % 9;
  }

  switch(start_square) {
    case 0: x = 0 - (rand() % BOX_SIZE);            y = 0 - (rand() & BOX_SIZE); break;
    case 1: x = (rand() % WINDOW_WIDTH);            y = 0 - (rand() & BOX_SIZE); break;
    case 2: x = WINDOW_WIDTH + (rand() % BOX_SIZE); y = 0 - (rand() & BOX_SIZE); break;

    case 3: x = 0 - (rand() % BOX_SIZE);            y = rand() % WINDOW_HEIGHT; break;
    case 4: x = (rand() % WINDOW_WIDTH);            y = rand() % WINDOW_HEIGHT; break;
    case 5: x = WINDOW_WIDTH + (rand() % BOX_SIZE); y = rand() % WINDOW_HEIGHT; break;

    case 6: x = 0 - (rand() % BOX_SIZE);            y = rand() % (WINDOW_HEIGHT + BOX_SIZE); break;
    case 7: x = (rand() % WINDOW_WIDTH);            y = rand() % (WINDOW_HEIGHT + BOX_SIZE); break;
    case 8: x = WINDOW_WIDTH + (rand() % BOX_SIZE); y = rand() % (WINDOW_HEIGHT + BOX_SIZE); break;
    default: break;
  }

  std::cout << "start square:" << start_square << ",x = " << x << ", y = " << y << std::endl;
}

void EndCoordinates(int &x, int &y, int &end_square, int start_square) {
  std::vector<int> end_square_choices;

  for (auto i = 0; i < 9; i++) {
    if (i != start_square)
      end_square_choices.push_back(i);
  }

  std::shuffle(end_square_choices.begin(), end_square_choices.end(), std::random_device());
  end_square = end_square_choices[0];

  StartCoordinates(x,y,end_square);
}

void XYincrements(int &xIncr, int &yIncr, int startX, int startY, int endX, int endY, int dCount) {
  xIncr = (endX > startX) ? (endX - startX) / dCount : -(startX - endX) / dCount;
  yIncr = (endY > startY) ? (endY - startY) / dCount : -(startY - endY) / dCount;
}

#define DOT(X,Y) coordinate(X,Y)
#define SQUARE(SIZE) DOT(0,0), DOT(SIZE,0), DOT(SIZE,SIZE), DOT(0,SIZE), DOT(0,0)
#define TRIANGLE(SIZE) DOT(SIZE,0), DOT(SIZE,SIZE/2), DOT(0,SIZE/2), DOT(SIZE/2,0)
#define HEXAGON(SIZE) DOT(0,SIZE/2), DOT(SIZE/2,0), DOT(SIZE,0), DOT(SIZE + SIZE/2,SIZE/2), DOT(SIZE,SIZE), DOT(SIZE/2,SIZE), DOT(0,SIZE/2)
#define STAR DOT(0,30), DOT(120,30), DOT(20,100), DOT(60,0), DOT(100,100), DOT(0,30)
#define ASTEROID1 DOT(60,0), DOT(100,10), DOT(120,50), DOT(90,80), DOT(60,70), DOT(50,90), DOT(20,60), DOT(40,40), DOT(30,30), DOT(60,0)

int main() {
  std::cout << "Test astObj class..." << std::endl;

  screen = tigrWindow(1024,1024,"Hello", 0);
  tigrClear(screen, scolor(BLACK) ); //tigrRGB(0x80, 0x90, 0xa0));
  tigrPrint(screen, tfont, 120, 110, tigrRGB(0xff, 0xff, 0xff), "Hello, world.");
  put_up_grid();


  std::vector<struct coordinate> my_outline{ ASTEROID1 };

  std::cout << "On screen instance. move it a few times within screen..." << std::endl;

  int startX, startY, startSquare = -1;
  StartCoordinates(startX, startY, startSquare);

  int endX, endY, endSquare = -1;
  EndCoordinates(endX, endY, endSquare, startSquare);

  int dCount = 40;

  int xIncr, yIncr;

  XYincrements(xIncr,yIncr,startX,startY,endX,endY,dCount);

  std::cout << "xIncr:" << xIncr << ",yIncr:" << yIncr << std::endl;
  
  class astObj my_astobj(&my_outline,startX,startY);

  my_astobj.SetTrajectory(xIncr,yIncr);

  
  bool do_test = true;

  //for(auto j = 0; j < 20; j++) {
    
  while(do_test) {
    for (auto i = 0; i < dCount; i++) {
      put_up_grid();

      std::cout << "PASS " << i << std::endl;

      my_astobj.Advance();
      tigrUpdate(screen);

      usleep(50000);
    }

    StartCoordinates(startX, startY, startSquare);
    EndCoordinates(endX, endY, endSquare, startSquare);

    xIncr = (endX > startX) ? (endX - startX) / dCount : -(startX - endX) / dCount;
    yIncr = (endY > startY) ? (endY - startY) / dCount : -(startY - endY) / dCount;

    std::cout << "xIncr:" << xIncr << ",yIncr:" << yIncr << std::endl;
    my_astobj.SetOrigin(startX,startY);
    my_astobj.SetTrajectory(xIncr,yIncr);
  }


  while (!tigrClosed(screen)) {
        tigrUpdate(screen);
  }

  tigrFree(screen);

  return 0;
}

