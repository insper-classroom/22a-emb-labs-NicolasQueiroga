#include "lab5.h"

// globals
volatile _Bool echo_flag = 0;
volatile _Bool contact_error_flag = 0;
volatile uint32_t rtt_time = 0;

// hw
void init_sensor(void)
{
    pio_configure(ECHO_PIO, PIO_INPUT, ECHO_PIO_IDX_MASK, PIO_DEFAULT);
    pio_set_debounce_filter(ECHO_PIO, ECHO_PIO_IDX_MASK, 60);
    pio_handler_set(ECHO_PIO,
                    ECHO_PIO_ID,
                    ECHO_PIO_IDX_MASK,
                    PIO_IT_EDGE,
                    echo_callback);
    pio_enable_interrupt(ECHO_PIO, ECHO_PIO_IDX_MASK);
    pio_get_interrupt_status(ECHO_PIO);
    NVIC_EnableIRQ(ECHO_PIO_ID);
    NVIC_SetPriority(ECHO_PIO_ID, 5);

    pmc_enable_periph_clk(TRIG_PIO_ID);
    pio_set_output(TRIG_PIO, TRIG_PIO_IDX_MASK, 0, 0, 1);
}

void sensor_trigger()
{
    pio_set(TRIG_PIO, TRIG_PIO_IDX_MASK);
    delay_us(10);
    pio_clear(TRIG_PIO, TRIG_PIO_IDX_MASK);
}

void echo_callback(void)
{
    if (pio_get(ECHO_PIO, PIO_INPUT, ECHO_PIO_IDX_MASK))
    {
        if (get_but2_flag())
        {
            RTT_init(FREQUENCY, 0, 0);
            echo_flag = 1;
        }
        else
            contact_error_flag = 1;
    }
    else
    {
        echo_flag = 0;
        rtt_time = rtt_read_timer_value(RTT);
    }
}

// sw
_Bool get_echo_flag(void)
{
    return echo_flag;
}
void set_echo_flag(_Bool val)
{
    echo_flag = val;
}

_Bool get_contact_error_flag(void)
{
    return contact_error_flag;
}
void set_contact_error_flag(_Bool val)
{
    contact_error_flag = val;
}

uint32_t get_rtt_time(void)
{
    return rtt_time;
}
void set_rtt_time(uint32_t val)
{
    rtt_time = val;
}

void draw_error(char type)
{
    clear_screen();
    if (type == 0)
        draw_string("DIST ERROR");
    else if (type == 1)
        draw_string("CON. ERROR");
}

void draw_distance(float distance)
{
    char str[20];
    sprintf(str, "%.1f cm", distance);
    clear_screen();
    gfx_mono_draw_string(str, 26, 11, &sysfont);
}