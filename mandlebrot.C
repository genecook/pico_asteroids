#include <complex>
#include <iostream>
#include <string>

#include <display.h>
#include <math.h>

#ifdef FOR_PICO
    #include <pico/util/queue.h>
    #include <pico/sem.h>
    #include <pico/multicore.h>

    // dumb down pico - pico floating pt is emulated and thus mandlebrot generation is quite slow
    #define ACCURACY 40
    #define FLOAT float

    // Defining the size of the screen.
    // to zoom, increase or decrease scale AND adjust to center as well...
    #define Y 480
    #define X 320
    #define CENTER_X 40
    #define CENTER_Y 140
    #define SCALE 2.4
#else
    #include <tigr.h>

    #define ACCURACY 100
    #define FLOAT double
    #define Y 1024
    #define X 1024
    #define CENTER_X 50
    #define CENTER_Y 220
    #define SCALE 2
#endif

//#define DEBUG_MANDELBROT 1

using namespace std;

#ifdef FOR_PICO
    #define FLAG_VALUE 123

    // task gated functioms (only one task at a time can access these functions)...
    semaphore_t display_char_sem; // used to insure only one core at a time writes to LCD

    void myDrawDot(int px, int py, unsigned R, unsigned G, unsigned B) {
        if ( (px < 0) || (px >= X) && (py < 0) && (py >= Y)) { 
            return; // dot is off screen...
        }
        if ( (R==0) && (G==R) && (B==R) ) {
            return; // ASSUME no need to draw black dot as screen is initially all black
        }
        sem_acquire_blocking(&display_char_sem);
        DrawDot(px,py,R,G,B);
        sem_release(&display_char_sem); 
    }
    // ...end of task gated functions...

#else
    unsigned int my_display_buffer[X][Y];

    void myDrawDot(int px, int py, unsigned R, unsigned G, unsigned B) {
        if ( (px >= 0) && (px < X) && (py >= 0) && (py < Y)) {
            my_display_buffer[px][py] = (R<<16) | (G<<8) | B;
        } else {
#ifdef DEBUG_MANDELBROT
        std::cout << "x/y: " << px << "/" << py << " R/G/B: " << std::hex 
                  << R << "/" << G << "/" << B << std::dec << std::endl;
#endif
        }
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
    }
#endif

// Recursive function to provide the iterative every Nth
// f^n (0) for every pixel on the screen.

#ifdef FOR_PICO 
    // lcd we are using implements 5 bits per color...
    #define NCOLORS_LO 16 
    #define NCOLORS_MASK 0x1f
#else
    #define NCOLORS_LO 128
    #define NCOLORS_MASK 0xff 
#endif

int plotX(complex<FLOAT> c) { return real(c) * Y / SCALE + X / SCALE + CENTER_X; }
int plotY(complex<FLOAT> c) { return imag(c) * Y / SCALE + Y / SCALE / 2 + CENTER_Y; }

#define LO_COLOR_INDEX ((int)(NCOLORS_LO - NCOLORS_LO * abs(t) / abs(c)) & 0xff)

int Mandle(complex<FLOAT> c, complex<FLOAT> t, int counter) {
    // To eliminate out of bound values.
    if (abs(t) > 4) {
        // can be used to 'fill in' low intensity colors, but I prefer black 'background'...
        //myDrawDot(plotX(c), plotY(c), LO_COLOR_INDEX, LO_COLOR_INDEX, LO_COLOR_INDEX);
        return 0;
    }
    // To put about the end of the fractal,
    // the higher the value of the counter,
    // The more accurate the fractal is generated,
    // however, higher values cause
    // more processing time.
    if (counter == ACCURACY) {
        myDrawDot(plotX(c), plotY(c), NCOLORS_MASK * (abs((t * t)) / abs((t - c) * c)), 0, 0);
        return 0;
    }

    // recursively calling Mandle with increased counter
    // and passing the value of the squares of t into it.
    Mandle(c, pow(t, 2) + c, counter + 1);
 
    return 0;
}

#ifdef FOR_PICO
    // limit somewhat the # of points to evaluate on pico. otherwise its just too slow...
    #define X_LO -1
    #define X_HI 1
    #define Y_LO -1
    #define Y_HI 1
    #define INCR 0.0019
#else
    #define X_LO -2
    #define X_HI 1
    #define Y_LO -2
    #define Y_HI 1
    #define INCR 0.0015
#endif

// task shared data...
#ifdef FOR_PICO
    semaphore_t task_data_sem;
#endif
    FLOAT x = X_LO;
    FLOAT y = Y_LO;
// ...end of task shared data

bool MandleSetPull() {
#ifdef FOR_PICO
    sem_acquire_blocking(&task_data_sem);
#endif
    // acquire current coordinates to evaluate, increment to next coordinate...
    FLOAT tx = x, ty = y;
    y += INCR;
    if (y >= Y_HI) {
        x += INCR;
        y = Y_LO;
    }
    FLOAT endx = x;

#ifdef FOR_PICO
    sem_release(&task_data_sem);
#endif

    if (endx >= X_HI)
        return false;
 
    complex<FLOAT> temp;
    temp.real(tx);
    temp.imag(ty);
    Mandle(temp,0,0);
    return true;
}

void MandleSet() {
#ifdef DEBUG_MANDELBROT
    long long scnt = 0, xcnt = 0;
    for (FLOAT x = -2; x < 2; x += INCR) {
        xcnt++;
        for (FLOAT y = -1; y < 1; y += INCR) {
            scnt++;
        }
    }
    std::cout << "# of Xsets/points to consider: " << xcnt << "/" << scnt << std::endl;
#endif
    // Calling Mandle function for every point on the screen.
    while(MandleSetPull()) {
        // more points to process...
    }
}

#ifdef FOR_PICO
void MandleSetCore1() {
  multicore_fifo_push_blocking(FLAG_VALUE);

  uint32_t g = multicore_fifo_pop_blocking();

  if (g == FLAG_VALUE) {
    // what we expected...
  } else {
    printf("ERROR, CORE 1 STARTUP???\n");
    return;
  }

  while(MandleSetPull()) {
    // more points to process...
  }
}
#endif

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

#ifdef FOR_PICO
    sem_init(&display_char_sem,1,1);
    sem_init(&task_data_sem,1,1);
    multicore_launch_core1(MandleSetCore1);
    uint32_t g = multicore_fifo_pop_blocking();

    if (g == FLAG_VALUE) {
        // what we expected...
        multicore_fifo_push_blocking(FLAG_VALUE);
    } else {
        printf("ERROR, CORE 0 STARTUP???\n");
        return 0;
    }

#endif

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

