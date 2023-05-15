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

#define MY_DISP_HOR_RES 480 
#define MY_DISP_VER_RES 320

#define CENTER_X MY_DISP_HOR_RES / 2
#define CENTER_Y MY_DISP_VER_RES / 2

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

static int tx = -1;
static int ty = -1;

lv_coord_t btn1_ulx;
lv_coord_t btn1_uly;
lv_coord_t btn1_lrx;
lv_coord_t btn1_lry;

void read_touch(lv_indev_t *indev, lv_indev_data_t *data) {
    int px,py;
    read_screen_touch(&px,&py);

    if ( (px > btn1_ulx) && (px < btn1_lrx) && (py > btn1_uly) && (py < btn1_lry) ) {
        data->point.x = px;
        data->point.y = py;
        data->state = LV_INDEV_STATE_PRESSED;
        printf("touch pressed!\n");
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

bool do_generate = false;

static void event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if ( (code == LV_EVENT_PRESSED) && !do_generate) {
        do_generate = true;
        printf("Pressed!\n");
    }
}

#define CANVAS_WIDTH  200
#define CANVAS_HEIGHT 200

static unsigned char canvas_buffer[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT)];

static lv_indev_t *indev;

int main() {
    stdio_init_all();
    InitializeDisplay("try lvgl!");

    lv_init();

    // create display buffer...
    disp = lv_disp_create( MY_DISP_HOR_RES, MY_DISP_VER_RES);
    lv_disp_set_rotation(disp, LV_DISP_ROTATION_90, 1 /* lvgl does rotation */); // or LV_DISP_ROT_180?
    lv_disp_set_flush_cb(disp, my_flush_cb);
    /*Initialize the display buffer.*/
    lv_disp_set_draw_buffers(disp, buf1, NULL, sizeof(buf1), LV_DISP_RENDER_MODE_PARTIAL); 

    // setup touch...
    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, read_touch);

    // setup canvas where mandlebrot will be plotted...
    lv_obj_t *canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, canvas_buffer, CANVAS_WIDTH, CANVAS_HEIGHT, LV_COLOR_FORMAT_NATIVE);
    lv_obj_center(canvas);
    lv_canvas_fill_bg(canvas, lv_color_hex3(0), LV_OPA_COVER);

    // token point on canvas 'til implementation complete...
    lv_canvas_set_px(canvas, 100,100, lv_color_hex3(0xccc), LV_OPA_COVER);

    // Change the active screen's background color...
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);

    // setup program 'banner' (label at top of screen with program name)...
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Pico Mandlebrott");
    lv_obj_set_style_text_color(lv_scr_act(), lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    // setup 'generate' button at bottom/middle of screen...
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
    lv_obj_add_event(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_t *btn1_label = lv_label_create(btn1);
    lv_label_set_text(btn1_label, "GO!!");
    lv_obj_center(btn1_label);

    lv_coord_t btn1_width  = lv_obj_get_width(btn1); 
    lv_coord_t btn1_height = lv_obj_get_height(btn1);
    lv_coord_t btn1_x      = lv_obj_get_x(btn1);
    lv_coord_t btn1_y      = lv_obj_get_y(btn1);

    btn1_ulx = btn1_x;
    btn1_uly = btn1_y;
    btn1_lrx = btn1_x + btn1_width;
    btn1_lry = btn1_y + btn1_height;

    // DO SOMETHING...

    // we'll use a spinner to denote that generation is in progress...
    lv_obj_t *generate_spinner;
    int generate_elapsed_time = 0; // for now just show some elapsed time

    int timer_cnt = 0;

/*
while (1)
{
  lv_timer_handler();
  HAL_Delay(5);
}i

lv_task_handler();???
lv_tick_task() ???
*/
    while(1) {
        // call timer 'tic' every 1k milliseconds...
        wait(100);
        lv_tick_inc(100);
        lv_timer_handler();
        // call lv timer handler every 5k milliseconds...
        //if ((timer_cnt += 1) >= 5) {
        //    lv_timer_handler();
        //    timer_cnt = 0;
        //}

        // after some time has elapsed, delete the spinner...
        
        if (generate_elapsed_time > 0) {
            generate_elapsed_time -= 1;
            if (generate_elapsed_time <= 0) {
                lv_obj_del(generate_spinner);
                do_generate = false;
                lv_obj_clear_state(btn1, LV_STATE_PRESSED);
            }
        } 

        if (do_generate && (generate_elapsed_time == 0)) {
            // put up spinner, generate, remove the spinner when done...
            generate_spinner = lv_spinner_create(lv_scr_act(), CENTER_X, CENTER_Y);
            lv_obj_set_size(generate_spinner, 50,50);
            lv_obj_center(generate_spinner);
            generate_elapsed_time = 100;
        }
    }

    CloseDisplay();
    return 0;
}
