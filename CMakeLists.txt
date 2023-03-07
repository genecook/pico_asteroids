cmake_minimum_required(VERSION 3.25)

# to build for pico:
#   cmake -DDO_PICO=ON ..

if (DEFINED DO_PICO)
    set(CMAKE_MODULE_PATH $ENV{PICO_SDK_PATH} $ENV{PICO_EXAMPLES_PATH})
    # this include has to happen BEFORE the project statement in order
    # to pick up the correct C compiler, etc. for targeting the pico...
    include(pico_sdk_import)
   # use shell variable and/or update this path to reflect where you
    # have unpacked the Waveshare LCD code:
    set(PICO_LCD_LIB $ENV{PICO_LCD_LIB})
    set(PROJECTS_PATH $ENV{PROJECTS_PATH})
    set(MY_PICO_GUI $ENV{PROJECTS_PATH}/pico_gui)
endif()

project(asteroids C CXX ASM)

# tigr - mac os/x specific easy-to-use graphics lib:
set(TIGR_DIR $ENV{PROJECTS_PATH}/tigr)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR} $ENV{PROJECTS_PATH}/tigr)

if (DEFINED DO_PICO)
    # raspberry pi pico build...
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -DPICO_DISPLAY -DFOR_PICO")

    pico_sdk_init()

    add_subdirectory(${PICO_LCD_LIB}/lib/config lib/config)
    add_subdirectory(${PICO_LCD_LIB}/lib/lcd lib/lcd)
    add_subdirectory(${PICO_LCD_LIB}/lib/font lib/font)
    add_subdirectory(${PICO_LCD_LIB}/examples examples)
    add_subdirectory(${PICO_LCD_LIB}/lib/fatfs lib/fatfs)
    add_subdirectory(${PICO_LCD_LIB}/lib/sdcard lib/sdcard)
    
    include_directories(${PICO_LCD_LIB}/examples)
    include_directories(${PICO_LCD_LIB}/lib/config)
    include_directories(${PICO_LCD_LIB}/lib/lcd)
    include_directories(${PICO_LCD_LIB}/lib/font)
    include_directories(${PICO_LCD_LIB}/lib/fatfs)
    include_directories(${PICO_LCD_LIB}/lib/sdcard)

    include_directories(${PROJECTS_PATH}/pico_gui)

    add_executable(test_astobj main.C asteroid_test.C ast_obj.C display.C ${MY_PICO_GUI}/lcd_touch_wrapper.c 
        ${MY_PICO_GUI}/my_fatfs_storage.c ${MY_PICO_GUI}/my_track_touch.c)
    
    target_link_libraries(test_astobj examples lcd font config pico_stdlib hardware_spi fatfs)

    pico_enable_stdio_usb(test_astobj 0)
    pico_enable_stdio_uart(test_astobj 1)
    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(test_astobj)
else() 
    # mac os/x build...
    set(CMAKE_EXE_LINKER_FLAGS "-framework OpenGL -framework Cocoa")
    add_executable(test_astobj main.C asteroid_test.C ast_obj.C display.C ${TIGR_DIR}/tigr.C)

    add_executable(mandelbrot_demo mandlebrot.C display.C ${TIGR_DIR}/tigr.C)
endif()
