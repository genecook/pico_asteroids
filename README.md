Using Waveshare 3.5 LCD/touch panel, as configured for Raspberry Pi Pico,
along with Waveshare UPS, also configured for the Pico.

Heres a list of the demo programs in this repo:

    1. test_astobj - randomly move an 'asteroid' around a grid. The asteroid is depicted using simple
    vector graphics. A version of the classic 'Evans-Sutherland' line clipping algorithm is used to
    clip the asteroid line segments to a window. This program was used to debug the line drawing
    code.

    The cmake (CMakeLists.txt) file can build this demo for mac os/x, or for the raspberry pi pico.

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
