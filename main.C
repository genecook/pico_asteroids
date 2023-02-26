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
#include <display.h>

//#define MY_DEBUG

void StartCoordinates(int &x, int &y, int &start_square) {
  if (start_square < 0) {
    while(start_square != 4) {    // ugly...
      start_square = rand() % 9;
    }
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

#ifdef MY_DEBUG
  std::cout << "start square:" << start_square << ",x = " << x << ", y = " << y << std::endl;
  #endif
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
#ifdef MY_DEBUG
  std::cout << "Test astObj class..." << std::endl;
#endif

  InitializeDisplay();

  std::vector<struct coordinate> my_outline{ ASTEROID1 };

#ifdef MY_DEBUG
  std::cout << "On screen instance. move it a few times within screen..." << std::endl;
#endif

  int startX, startY, startSquare = -1;
  StartCoordinates(startX, startY, startSquare);

  int endX, endY, endSquare = -1;
  EndCoordinates(endX, endY, endSquare, startSquare);

  int dCount = 40;

  int xIncr, yIncr;

  XYincrements(xIncr,yIncr,startX,startY,endX,endY,dCount);

#ifdef MY_DEBUG
  std::cout << "xIncr:" << xIncr << ",yIncr:" << yIncr << std::endl;
#endif

  class astObj my_astobj(&my_outline,startX,startY);

  my_astobj.SetTrajectory(xIncr,yIncr);

  
  bool do_test = true;

  while(do_test) {
    for (auto i = 0; i < dCount; i++) {
#ifdef MY_DEBUG
      std::cout << "PASS " << i << std::endl;
#endif
      my_astobj.Advance();

      UpdateDisplay();

      DrawDelay();
    }

    StartCoordinates(startX, startY, startSquare);
    EndCoordinates(endX, endY, endSquare, startSquare);

    xIncr = (endX > startX) ? (endX - startX) / dCount : -(startX - endX) / dCount;
    yIncr = (endY > startY) ? (endY - startY) / dCount : -(startY - endY) / dCount;

#ifdef MY_DEBUG
    std::cout << "xIncr:" << xIncr << ",yIncr:" << yIncr << std::endl;
#endif
    my_astobj.SetOrigin(startX,startY);
    my_astobj.SetTrajectory(xIncr,yIncr);
  }

  CloseDisplay();

  return 0;
}

