#ifdef FOR_PICO
//#include "hardware/watchdog.h"
#include "pico/stdlib.h"
extern "C" {
#include <lcd_touch_wrapper.h>
}
#else
#include <stdlib.h>
#endif

#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <time.h>
#include <unistd.h>

#include <display.h>
#include <ast_obj.h>

//#define MY_DEBUG

// start/end coordinates for random asteroid placement...

void StartCoordinates(int &x, int &y, int &start_square) {
  if (start_square < 0) {
    start_square = rand() / (RAND_MAX / 9 + 1);
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

// our 'meteors' (are intended to) fall from upper left hand of window thru lower right...
void MeteorStartEndCoordinates(int &startX, int &startY, int &endX, int &endY, int lane = -1) {
  int start_square = (lane >= 0) ? lane : rand() / (RAND_MAX / 3 + 1);

    // lanes 0, 1, 2 (grid squares 0, 1, or 3)...
    switch(start_square) {
      case 0: startX = 0 - (rand() % BOX_SIZE); startY = 0 - (rand() & BOX_SIZE); break;
      case 1: startX = (rand() % WINDOW_WIDTH); startY = 0 - (rand() & BOX_SIZE); break;
      case 2: startX = 0 - (rand() % BOX_SIZE); startY = rand() % WINDOW_HEIGHT;  break;
      default: break;
    }

    endX = startX + WINDOW_WIDTH + BOX_SIZE;
    endY = startY + WINDOW_HEIGHT + BOX_SIZE;
}

#define DOT(X,Y) coordinate(X,Y)
#define SQUARE(SIZE) DOT(0,0), DOT(SIZE,0), DOT(SIZE,SIZE), DOT(0,SIZE), DOT(0,0)
#define TRIANGLE(SIZE) DOT(SIZE,0), DOT(SIZE,SIZE/2), DOT(0,SIZE/2), DOT(SIZE/2,0)
#define HEXAGON(SIZE) DOT(0,SIZE/2), DOT(SIZE/2,0), DOT(SIZE,0), DOT(SIZE + SIZE/2,SIZE/2), DOT(SIZE,SIZE), DOT(SIZE/2,SIZE), DOT(0,SIZE/2)
#define STAR DOT(0,30), DOT(120,30), DOT(20,100), DOT(60,0), DOT(100,100), DOT(0,30)

#define ASTEROID1 DOT(60,0), DOT(100,10), DOT(120,50), DOT(90,80), DOT(60,70), DOT(50,90), DOT(20,60), DOT(40,40), DOT(30,30), DOT(60,0)
#define ASTEROID2 DOT(60,0), DOT(100,10),  DOT(120,50), DOT(90,80), DOT(50,90), DOT(20,60), DOT(30,20), DOT(60,0)

//******************************************************************************************
// randomly place/move one asteroid on grid, crossing window boundaries, to test the
// line clipping code...
//******************************************************************************************

void asteroid_test() {
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

    startSquare = -1;
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
}

//******************************************************************************************
// lets do some meteors...
//******************************************************************************************

std::vector<struct coordinate> meteor1_outline{ ASTEROID1 };
std::vector<struct coordinate> meteor2_outline{ ASTEROID2 };

#define DEBUG_METEORS 1

void PlaceMeteor(class astObj &my_meteor, int dCount, int lane = -1) {
  int startX, startY, endX, endY;
  MeteorStartEndCoordinates(startX, startY, endX, endY, lane);

  int xIncr, yIncr;

  XYincrements(xIncr,yIncr,startX,startY,endX,endY,dCount);

  if ( (rand() & 1) == 1 ) {
    my_meteor.AddOutline(&meteor1_outline);
  } else {
    my_meteor.AddOutline(&meteor2_outline);
  }

  my_meteor.SetOrigin(startX, startY);
  my_meteor.SetTrajectory(xIncr,yIncr);

  float scale = 1.0;

  switch((int) rand() & 3) {
    case 0: break;
    case 1: scale = 0.25; break;
    case 2: scale = 0.50; break;
    case 3: scale = 0.75; break;
    default: break;
  }

  float rotation = 0.0;
/*
  switch((int) rand() & 3) {
    case 0: break;
    case 1: rotation = 5.0;  break;
    case 2: rotation = 10.0; break;
    case 3: rotation = 15.0; break;
    default: break;
  }
*/
  my_meteor.SetScaleRotation(scale,rotation);

  my_meteor.SetCountdown( rand() / (RAND_MAX / 9 + 1) );
#ifdef DEBUG_METEORS
  std::cout << "\tcount down value: " << my_meteor.Countdown() << std::endl;
#endif
//  std::cout << "meteor placed at x/y: " << startX << "/" << startY
//    << " x/y incrs: " << xIncr << "/" << yIncr << "..." << std::endl;
}

#define METEORS_IN_FLIGHT 5

void meteor_shower() {
    std::vector<class astObj> my_meteors;

    unsigned int clock_ticker = 0;

    int dCount = 40;  

    bool clock_running = true;

    while(clock_running) {
#ifdef DEBUG_METEORS
      //std::cout << "tick..." << clock_ticker << std::endl; 
#endif
      clock_ticker++;
#ifdef DEBUG_METEORS
      //std::cout << "there are " << my_meteors.size() << " meteors" << std::endl;
#endif

      for (auto mi = my_meteors.begin(); mi != my_meteors.end();) {
#ifdef DEBUG_METEORS
        //std::cout << "processing next meteor at time " << clock_ticker << "..." << std::endl;
#endif
        if (!mi->Advance()) {
          // no action, this meteor, this time. no need to evaluate/update its state...
          mi++;
          continue;
        }

        // this meteor moved at this clock interval...

        if (!mi->OnScreen() && mi->Visible()) {
#ifdef DEBUG_METEORS
          std::cout << "falling meteor is now visible..." << std::endl;
#endif
          mi->SetOnScreen(); // falling meteor is now visible... 
          mi++;
          continue;
        }

        if (mi->OnScreen() && !mi->Visible()) {
#ifdef DEBUG_METEORS
          std::cout << "meteor no longer visible..." << std::endl;
#endif
          my_meteors.erase(mi); // meteor no longer visible...
          continue;
        }
          
        mi++;
      }

      if (my_meteors.size() < METEORS_IN_FLIGHT) {
#ifdef DEBUG_METEORS
        std::cout << "adding new meteor..." << std::endl;
#endif
        my_meteors.push_back( astObj() );
        PlaceMeteor(my_meteors.back(), dCount);
      }
      UpdateDisplay();
      DrawDelay();
      //if (clock_ticker == 200) clock_running = false;
    }
}

void simple_meteor_shower() {
  int dCount = 40;  
  bool do_test = true;

  while(do_test) {
    std::vector<class astObj> my_meteors = { astObj(), astObj() };
    PlaceMeteor(my_meteors[0], dCount, 1);
    PlaceMeteor(my_meteors[1], dCount, 2);

    // random starting coordinates of meteor and its trajectory usually
    // cause it to be visible at some point in time...
    for(auto i = 0; (i < dCount) && !my_meteors[0].Visible(); i++) {
      my_meteors[0].Advance();
    }
    // but not always...
    if (!my_meteors[0].Visible())
      continue;
#ifdef DEBUG_METEORS
    std::cout << "\tmeteor is visible..." << std::endl;
#endif
    while( my_meteors[0].Visible() ) {
      my_meteors[0].Advance();
      my_meteors[1].Advance();
      UpdateDisplay();
      DrawDelay();
    }

    for(auto i = 0; (i < dCount) && my_meteors[1].Visible(); i++) {
      my_meteors[1].Advance();
      UpdateDisplay();
      DrawDelay();
    }
#ifdef DEBUG_METEORS
    std::cout << "meteor is gone!" << std::endl;
#endif
  }
}
