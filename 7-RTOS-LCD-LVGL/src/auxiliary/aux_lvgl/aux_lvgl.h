#ifndef __AUX_LVGL_H_
#define __AUX_LVGL_H_

#include "auxiliary/aux_hw/aux_hw.h"
#include <string.h>
#include "lvgl.h"
#include "ili9341.h"
#include "touch/touch.h"
LV_FONT_DECLARE(dseg70);
LV_FONT_DECLARE(dseg50);
LV_FONT_DECLARE(dseg35);
LV_FONT_DECLARE(dseg20);

#define LV_HOR_RES_MAX (320)
#define LV_VER_RES_MAX (240)
#define LV_HOR_RES_MAX (320)
#define LV_VER_RES_MAX (240)

// labels getters
lv_obj_t *get_label_floor(void);
lv_obj_t *get_label_clock(void);
lv_obj_t *get_label_temp(void);

// event handlers
void event_handler(lv_event_t *e);
void up_handler(lv_event_t *e);
void dn_handler(lv_event_t *e);

// screen
void lv_termostato(void);

// setup
void my_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
void my_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data);
void configure_lvgl(void);
void configure_lcd(void);

#endif