#include "main.h"
//#include "hardware/watchdog.h"
#include "pico/stdlib.h"

extern "C" {
    #include <lcd_touch_wrapper.h>
}
#include <stdio.h>

#include <iostream>
#include <string>

#include <display.h>

#include "lvgl.h"

#define MY_DISP_HOR_RES 320
#define MY_DISP_VER_RES 480

// Declare screen buffer...
static lv_color_t buf1[MY_DISP_HOR_RES * 10];                        

static lv_disp_t *disp;

void my_flush_cb(lv_disp_t * disp, const lv_area_t * area, lv_color_t * buf) {
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one
     *`put_px` is just an example, it needs to be implemented by you.*/

    int32_t x, y;

    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            //put_px(x, y, *color_p);
            DrawDot(x, MY_DISP_VER_RES - y, ((lv_color16_t) *buf).red, ((lv_color16_t) *buf).green, ((lv_color16_t) *buf).blue ); 
            buf++;
        }
    }

    /* IMPORTANT!!!
     * Inform LVGL that you are ready with the flushing and buf is not used anymore*/
    lv_disp_flush_ready(disp);
}

void  lv_example_get_started_1(void) {
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello World!");
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

int main() {
    stdio_init_all();
    InitializeDisplay("try lvgl!");

    lv_init();

    /* create display buffer...*/

    disp = lv_disp_create( MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_disp_set_flush_cb(disp, my_flush_cb);
    /*Initialize the display buffer.*/
    lv_disp_set_draw_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISP_RENDER_MODE_PARTIAL); 

    // DO SOMETHING...

    lv_example_get_started_1();

    int timer_cnt = 0;

    while(1) {
        // call timer 'tic' every 1k milliseconds...
        wait(1000);
        lv_tick_inc(1);
        // call lv timer handler every 5k milliseconds...
        if ((timer_cnt += 1) >= 5) {
            lv_timer_handler();
            timer_cnt = 0;
        }
    }

    CloseDisplay();
    return 0;
}
