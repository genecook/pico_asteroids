#include <complex>
#include <iostream>
#include <string>

#include <display.h>

#ifdef FOR_PICO
#else
#include <tigr.h>
#endif

//#define DEBUG_MANDELBROT 1

// Defining the size of the screen.
// to zoom, increase screen size AND adjust to center as well...

#ifdef FOR_PICO
#define Y 320
#define X 480
#define CENTER_X 0
#define CENTER_Y 0
#else
#define Y 1024
#define X 1024
#define CENTER_X 0
#define CENTER_Y 0
#endif

using namespace std;

#ifdef FOR_PICO
void myDrawDot(int px, int py, unsigned R, unsigned G, unsigned B) {
    DrawDot(px,py,R,G,B);
}
#else
unsigned int my_display_buffer[X][Y];

int lo_x = 0, hi_x = 0, lo_y = 0, hi_y = 0;
void myDrawDot(int px, int py, unsigned R, unsigned G, unsigned B) {
    if ( (px >= 0) && (px < X) && (py >= 0) && (py < Y)) {
        my_display_buffer[px][py] = (R<<16) | (G<<8) | B;
    } else {
#ifdef DEBUG_MANDELBROT
        std::cout << "x/y: " << px << "/" << py << " R/G/B: " << std::hex 
                  << R << "/" << G << "/" << B << std::dec << std::endl;
#endif
    }
    if (px < lo_x) lo_x = px;
    else if (px > hi_x) hi_x = px;
    if (py < lo_y) lo_y = py;
    else if (py > hi_y) hi_y = py;
}

void dumpDisplay() {
    for(auto x = 0; x < X; x++) {
        for(auto y = 0; y < Y; y++) {
            unsigned R = (my_display_buffer[x][y] >> 16) & 0xff;
            unsigned G = (my_display_buffer[x][y] >> 8) & 0xff;
            unsigned B = my_display_buffer[x][y] & 0xff;
            DrawDot(x,y,R,G,B);
        }
    }
#ifdef DEBUG_MANDELBROT
    std::cout << "X range: " << lo_x << "/" << hi_x << " y:" << lo_y << "/" << hi_y << std::endl; 
#endif
}
#endif

// Recursive function to provide the iterative every 100th
// f^n (0) for every pixel on the screen.

int Mandle(complex<double> c, complex<double> t, int counter) {
    // To eliminate out of bound values.
    if (abs(t) > 4) {
        myDrawDot(real(c) * Y / 2 + X / 2 + CENTER_X, 
                imag(c) * Y / 2 + Y / 2 + CENTER_Y,
                 128 - 128 * abs(t) / abs(c), 
                 128 - 128 * abs(t) / abs(c), 
                 128 - 128 * abs(t) / abs(c));
        return 0;
    }
    // To put about the end of the fractal,
    // the higher the value of the counter,
    // The more accurate the fractal is generated,
    // however, higher values cause
    // more processing time.
    if (counter == 100) {
        myDrawDot(real(c) * Y / 2 + X / 2 + CENTER_X, 
                imag(c) * Y / 2 + Y / 2 + CENTER_Y, 
                    255 * (abs((t * t)) / abs((t - c) * c)), 0, 0);
        return 0;
    }

    // recursively calling Mandle with increased counter
    // and passing the value of the squares of t into it.
    Mandle(c, pow(t, 2) + c, counter + 1);
 
    return 0;
}

//#define INCR 0.0015
// xbounds -2 .. 2
// ybounds -1 .. 1
#define X_LO -2
#define X_HI 2
#define Y_LO -1
#define Y_HI 1

#define INCR 0.0015

int MandleSet() {
    long long scnt = 0, xcnt = 0;
    for (double x = -2; x < 2; x += INCR) {
        xcnt++;
        for (double y = -1; y < 1; y += INCR) {
            scnt++;
        }
    }
#ifdef DEBUG_MANDELBROT
    std::cout << "# of Xsets/points to consider: " << xcnt << "/" << scnt << std::endl;
#endif
    // Calling Mandle function for every point on the screen.
    //unsigned long long xi = 0;
    for (double x = X_LO; x < X_HI; x += INCR) {
        for (double y = Y_LO; y < Y_HI; y += INCR) {
            complex<double> temp;
            temp.real(x);
            temp.imag(y);
            Mandle(temp,0,0);
        }
    }

    return 0;
}

/************************************************************************
 * main...
*************************************************************************/

#ifdef FOR_PICO
extern "C" {
    void read_screen_touch(int *x, int *y);
    void wait(unsigned int milliseconds);
}
#endif

int main() {
    InitializeDisplay("meteors!");

    MandleSet();

#ifdef FOR_PICO
    // add while loop...
    bool waiting = true;
    int touchX, touchY;
    while(waiting) {
        DrawDelay();
        read_screen_touch(&touchX,&touchY);
        wait(1000);
    }
#else
    std::cout << "dumping display buffer to screen..." << std::endl;
    dumpDisplay();
    UpdateDisplay();
    std::cout << "done!" << std::endl;

    getchar(); // wait for input...
#endif

    CloseDisplay();

    return 0;
}

 