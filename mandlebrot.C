#include <complex>
#include <complex.h>
#include <iostream>
#include <string>

#include <display.h>
#include <tigr.h>

// Defining the size of the screen.
#define Y 1024
#define X 1024

using namespace std;

unsigned int my_display_buffer[X][Y];

int lo_x = 0, hi_x = 0, lo_y = 0, hi_y = 0;
void myDrawDot(int px, int py, unsigned R, unsigned G, unsigned B) {
    if ( (px >= 0) && (px < X) && (py >= 0) && (py < Y)) {
        my_display_buffer[px][py] = (R<<16) | (G<<8) | B;
    } else {
        //std::cout << "x/y: " << px << "/" << py << " R/G/B: " << std::hex 
        //            << R << "/" << G << "/" << B << std::dec << std::endl;
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
    //std::cout << "X range: " << lo_x << "/" << hi_x << " y:" << lo_y << "/" << hi_y << std::endl; 
}
// Recursive function to provide the iterative every 100th
// f^n (0) for every pixel on the screen.

int Mandle(complex<double> c, complex<double> t, int counter) {
    //if (imag(c) != 0)
    //   std::cout << "c: " << c << " t: " << t << " counter: " << counter << std::endl;
    //UpdateDisplay();
    // To eliminate out of bound values.
    if (abs(t) > 4) {
        myDrawDot(real(c) * Y / 2 + X / 2, 
                imag(c) * Y / 2 + Y / 2,
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
        myDrawDot(real(c) * Y / 2 + X / 2, 
                imag(c) * Y / 2 + Y / 2, 
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
#define INCR 0.0015

int MandleSet() {
    long long scnt = 0, xcnt = 0;
    for (double x = -2; x < 2; x += INCR) {
        xcnt++;
        for (double y = -1; y < 1; y += INCR) {
            scnt++;
        }
    }
    std::cout << "# of Xsets/points to consider: " << xcnt << "/" << scnt << std::endl;

    // Calling Mandle function for every point on the screen.
    unsigned long long xi = 0;
    for (double x = -2; x < 2; x += INCR) {
        std::cout << "Xset " << xi++ << " out of " << xcnt << " sets..." << std::endl;
        for (double y = -1; y < 1; y += INCR) {
            complex<double> temp; // x + y * _Complex_I; 
            temp.real(x);
            temp.imag(y);
            Mandle(temp,0,0);
            //UpdateDisplay();
        }
    }

    return 0;
}

/************************************************************************
 * main...
*************************************************************************/

int main() {
    InitializeDisplay("meteors!");

    MandleSet();
    std::cout << "dumping display buffer to screen..." << std::endl;
    dumpDisplay();
    UpdateDisplay();
    std::cout << "done!" << std::endl;
    getchar();

    CloseDisplay();

    return 0;
}

 