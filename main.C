#ifdef FOR_PICO
#include "main.h"
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

#include <display.h>
#include <ast_obj.h>

//#define MY_DEBUG

void asteroid_test();

//******************************************************************************************
// main entry point...
//******************************************************************************************

int main() {
#ifdef FOR_PICO
  stdio_init_all();
#endif

#ifdef MY_DEBUG
  std::cout << "Test astObj class..." << std::endl;
#endif

#ifdef FOR_PICO
  // how to set initial random seed on pico???
#else
  sranddev();
#endif

  InitializeDisplay();
  asteroid_test();
  CloseDisplay();

  return 0;
}

