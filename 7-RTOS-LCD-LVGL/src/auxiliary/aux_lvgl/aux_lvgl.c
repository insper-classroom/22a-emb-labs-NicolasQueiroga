#include "aux_lvgl.h"
#include "auxiliary/aux_rtos/aux_rtos.h"
#include "auxiliary/aux_time/aux_time.h"
#include "clock.h"
#include "steam.h"

// global variables
volatile _Bool edit_clk = 0;
volatile _Bool display_off = 0;

// buffers
static lv_disp_draw_buf_t disp_buf;
static lv_color_t buf_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];
static lv_disp_drv_t disp_drv; /*A variable to hold the drivers. Must be static or global.*/
static lv_indev_drv_t indev_drv;

// labels
static lv_obj_t *labelPwr;
static lv_obj_t *labelMenu;
static lv_obj_t *labelConfig;
static lv_obj_t *labelUp;
static lv_obj_t *labelDn;
static lv_obj_t *labelFloor;
static lv_obj_t *labelFloor_dec;
static lv_obj_t *labelClock;
static lv_obj_t *labelTemp;
static lv_obj_t *labelFloor_txt;
static lv_obj_t *labelSetTemp_txt;
static lv_obj_t *labelSettings;
static lv_obj_t *labelHouseLogo;

// labels getters
lv_obj_t *get_label_floor(void) { return labelFloor; }
lv_obj_t *get_label_clock(void) { return labelClock; }
lv_obj_t *get_label_temp(void) { return labelTemp; }

// event handlers
void event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        LV_LOG_USER("Clicked");
    }
    else if (code == LV_EVENT_VALUE_CHANGED)
    {
        LV_LOG_USER("Toggled");
    }
}

void display_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        display_off = !display_off;
        if (!display_off)
        {
            lv_obj_clean(lv_scr_act());
            screen_off();
        }
        else
        {
            lv_obj_clean(lv_scr_act());
            lv_termostato();
        }
    }
}

void up_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    char *c;
    int temp;
    uint32_t current_hour, current_min, current_sec;
    if (code == LV_EVENT_CLICKED)
    {
        if (!edit_clk)
        {
            c = lv_label_get_text(labelTemp);
            temp = atoi(c);
            lv_label_set_text_fmt(labelTemp, "%02d", temp + 1);
        }
        else
        {
            rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
            if (current_min == 59)
            {
                current_min = 0;
                current_hour++;
            }
            else
                current_min++;
            rtc_set_time(RTC, current_hour, current_min, current_sec);
        }
    }
}

void dn_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    char *c;
    int temp;
    uint32_t current_hour, current_min, current_sec;
    if (code == LV_EVENT_CLICKED)
    {
        if (!edit_clk)
        {
            c = lv_label_get_text(labelTemp);
            temp = atoi(c);
            lv_label_set_text_fmt(labelTemp, "%02d", temp - 1);
        }
        else
        {
            rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
            if (current_min == 0)
            {
                current_min = 59;
                current_hour--;
            }
            else
                current_min--;
            rtc_set_time(RTC, current_hour, current_min, current_sec);
        }
    }
}

void set_clk_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
        edit_clk = !edit_clk;
}

void create_btn(lv_obj_t *parent, lv_obj_t *label, lv_event_cb_t event_handler, lv_obj_t **btn, lv_event_code_t filter, int width, int height)
{
    *btn = lv_btn_create(parent);
    lv_obj_set_size(*btn, width, height);
    lv_obj_add_event_cb(*btn, event_handler, filter, NULL);
}

// screen
void screen_off(void)
{
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_NONE));
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_NONE));
    lv_style_set_border_width(&style, 5);

    lv_obj_t *pwr;
    create_btn(lv_scr_act(), labelPwr, display_handler, &pwr, LV_EVENT_ALL, 60, 60);
    lv_obj_align(pwr, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_style(pwr, &style, 0);

    labelPwr = lv_label_create(pwr);
    lv_label_set_text(labelPwr, LV_SYMBOL_POWER);
    lv_obj_center(labelPwr);
}

void lv_termostato(void)
{
    lv_obj_clean(lv_scr_act());

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_NONE));
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_NONE));
    lv_style_set_border_width(&style, 5);

    //

    lv_obj_t *pwr;
    create_btn(lv_scr_act(), labelPwr, display_handler, &pwr, LV_EVENT_ALL, 60, 60);
    lv_obj_align(pwr, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_style(pwr, &style, 0);

    labelPwr = lv_label_create(pwr);
    lv_label_set_text(labelPwr, "[ " LV_SYMBOL_POWER);
    lv_obj_center(labelPwr);

    //

    lv_obj_t *menu;
    create_btn(lv_scr_act(), labelMenu, event_handler, &menu, LV_EVENT_ALL, 60, 60);
    lv_obj_align_to(menu, pwr, LV_ALIGN_OUT_RIGHT_TOP, -5, 0);
    lv_obj_add_style(menu, &style, 0);

    labelMenu = lv_label_create(menu);
    lv_label_set_text(labelMenu, "| M");
    lv_obj_center(labelMenu);

    //

    // lv_obj_t *config;
    // create_btn(lv_scr_act(), labelConfig, set_clk_handler, &config, LV_EVENT_ALL, 60, 60);
    // lv_obj_align_to(config, menu, LV_ALIGN_OUT_RIGHT_TOP, 0, 2);
    // lv_obj_add_style(config, &style, 0);

    lv_obj_t *clk = lv_imgbtn_create(lv_scr_act());
    lv_obj_add_event_cb(clk, set_clk_handler, LV_EVENT_ALL, NULL);
	lv_imgbtn_set_src(clk, LV_IMGBTN_STATE_RELEASED, &clock, NULL, NULL);
	lv_obj_align_to(clk, menu, LV_ALIGN_OUT_RIGHT_MID, 0, 45);

    // labelConfig = lv_label_create(config);
    // lv_label_set_text(labelConfig, LV_SYMBOL_SETTINGS " ]");
    // lv_obj_center(labelConfig);

    //

    lv_obj_t *up;
    create_btn(lv_scr_act(), labelUp, up_handler, &up, LV_EVENT_ALL, 60, 60);
    lv_obj_align_to(up, clk, LV_ALIGN_OUT_RIGHT_TOP, -48, -14);
    lv_obj_add_style(up, &style, 0);

    labelUp = lv_label_create(up);
    lv_label_set_text(labelUp, "[ " LV_SYMBOL_UP);
    lv_obj_center(labelUp);

    //

    lv_obj_t *dn;
    create_btn(lv_scr_act(), labelDn, dn_handler, &dn, LV_EVENT_ALL, 60, 60);
    lv_obj_align_to(dn, up, LV_ALIGN_OUT_RIGHT_TOP, 5, 0);
    lv_obj_add_style(dn, &style, 0);

    labelDn = lv_label_create(dn);
    lv_label_set_text(labelDn, LV_SYMBOL_DOWN " ]");
    lv_obj_center(labelDn);

    //

    labelFloor = lv_label_create(lv_scr_act());
    lv_obj_align(labelFloor, LV_ALIGN_LEFT_MID, 25, -20);
    lv_obj_set_style_text_font(labelFloor, &dseg70, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(labelFloor, lv_color_white(), LV_STATE_DEFAULT);
    lv_label_set_text_fmt(labelFloor, "%02d", 23);

    labelFloor_dec = lv_label_create(lv_scr_act());
    lv_obj_align_to(labelFloor_dec, labelFloor, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_text_font(labelFloor_dec, &dseg50, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(labelFloor_dec, lv_color_white(), LV_STATE_DEFAULT);
    lv_label_set_text_fmt(labelFloor_dec, ".%01d", 1);

    //

    labelClock = lv_label_create(lv_scr_act());
    lv_obj_align(labelClock, LV_ALIGN_TOP_RIGHT, -10, 10);
    lv_obj_set_style_text_font(labelClock, &dseg35, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(labelClock, lv_color_white(), LV_STATE_DEFAULT);
    lv_label_set_text_fmt(labelClock, "%02d:%02d", 0, 0);

    //

    labelTemp = lv_label_create(lv_scr_act());
    lv_obj_align_to(labelTemp, labelClock, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_set_style_text_font(labelTemp, &dseg50, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(labelTemp, lv_color_white(), LV_STATE_DEFAULT);
    lv_label_set_text_fmt(labelTemp, "%02d", 22);

    //

    labelFloor_txt = lv_label_create(lv_scr_act());
    lv_obj_align_to(labelFloor_txt, labelFloor, LV_ALIGN_OUT_LEFT_MID, 0, 0);
    lv_obj_set_style_text_font(labelFloor_txt, &dseg50, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(labelFloor_txt, lv_color_white(), LV_STATE_DEFAULT);
    lv_label_set_text_fmt(labelFloor_txt, "FLOOR\nTEMP");
}

// setup
void my_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    ili9341_set_top_left_limit(area->x1, area->y1);
    ili9341_set_bottom_right_limit(area->x2, area->y2);
    ili9341_copy_pixels_to_screen(color_p, (area->x2 + 1 - area->x1) * (area->y2 + 1 - area->y1));
    lv_disp_flush_ready(disp_drv);
}

void my_input_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    int px, py, pressed;
    if (readPoint(&px, &py))
        data->state = LV_INDEV_STATE_PRESSED;
    else
        data->state = LV_INDEV_STATE_RELEASED;

    data->point.x = px;
    data->point.y = py;
}

void configure_lvgl(void)
{
    lv_init();
    lv_disp_draw_buf_init(&disp_buf, buf_1, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX);

    lv_disp_drv_init(&disp_drv);       /*Basic initialization*/
    disp_drv.draw_buf = &disp_buf;     /*Set an initialized buffer*/
    disp_drv.flush_cb = my_flush_cb;   /*Set a flush callback to draw to the display*/
    disp_drv.hor_res = LV_HOR_RES_MAX; /*Set the horizontal resolution in pixels*/
    disp_drv.ver_res = LV_VER_RES_MAX; /*Set the vertical resolution in pixels*/

    lv_disp_t *disp;
    disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/

    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_input_read;
    lv_indev_t *my_indev = lv_indev_drv_register(&indev_drv);
}

void configure_lcd(void)
{
    pio_configure_pin(LCD_SPI_MISO_PIO, LCD_SPI_MISO_FLAGS);
    pio_configure_pin(LCD_SPI_MOSI_PIO, LCD_SPI_MOSI_FLAGS);
    pio_configure_pin(LCD_SPI_SPCK_PIO, LCD_SPI_SPCK_FLAGS);
    pio_configure_pin(LCD_SPI_NPCS_PIO, LCD_SPI_NPCS_FLAGS);
    pio_configure_pin(LCD_SPI_RESET_PIO, LCD_SPI_RESET_FLAGS);
    pio_configure_pin(LCD_SPI_CDS_PIO, LCD_SPI_CDS_FLAGS);

    ili9341_init();
    ili9341_backlight_on();
}