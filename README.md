Using Waveshare 3.5 LCD/touch panel, as configured for Raspberry Pi Pico,
along with Waveshare UPS, also configured for the Pico.

Heres a list of the demo programs in this repo:

    1. test_astobj - randomly move an 'asteroid' around a grid. The asteroid is depicted using simple
    vector graphics. A version of the classic 'Evans-Sutherland' line clipping algorithm is used to
    clip the asteroid line segments to a window. This program was used to debug the line drawing
    code.

    2. mandlebrot_demo - generate mandlebrot image. The points/accuracy are dumbed down somewhat
    on the pico. Both cores on the pico are employed in effort to speed up the generation.
 
    Both of these demo programs make use of the excellent tigr (https://github.com/erkkah/tigr.git) graphics library.

    The cmake (CMakeLists.txt) file can build these two demo for mac os/x, or for the raspberry pi pico.

    3. try_lvgl - example program using LVGL graphics library. 
    
To build the mac os/x executable:
    mkdir build
    cd build
    cmake ..
    make

To build the raspberry pi pico executable*:

    mkdir build_pico
    cd build_pico
    cmake -DDO_PICO=on ..

*see CMakeLists.txt file for required shell variables/checkouts required.
