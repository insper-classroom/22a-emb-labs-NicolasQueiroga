#include "auxiliary.h"

// globals
volatile _Bool but2_flag = 0;

// hw
// inits ----
void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource)
{
    uint16_t pllPreScale = (int)(((float)32768) / freqPrescale);

    rtt_sel_source(RTT, false);
    rtt_init(RTT, pllPreScale);

    if (rttIRQSource & RTT_MR_ALMIEN)
    {
        uint32_t ul_previous_time;
        ul_previous_time = rtt_read_timer_value(RTT);
        while (ul_previous_time == rtt_read_timer_value(RTT))
            ;
        rtt_write_alarm_time(RTT, IrqNPulses + ul_previous_time);
    }

    /* config NVIC */
    NVIC_DisableIRQ(RTT_IRQn);
    NVIC_ClearPendingIRQ(RTT_IRQn);
    NVIC_SetPriority(RTT_IRQn, 4);
    NVIC_EnableIRQ(RTT_IRQn);

    /* Enable RTT interrupt */
    if (rttIRQSource & (RTT_MR_RTTINCIEN | RTT_MR_ALMIEN))
        rtt_enable_interrupt(RTT, rttIRQSource);
    else
        rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_ALMIEN);
}

void init(void)
{
    // setup
    board_init();
    sysclk_init();
    delay_init();
    gfx_mono_ssd1306_init();
    WDT->WDT_MR = WDT_MR_WDDIS;

    // init clocks
    pmc_enable_periph_clk(ID_PIOC);

    // btn2
    pio_configure(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
    pio_set_debounce_filter(BUT2_PIO, BUT2_PIO_IDX_MASK, 60);
    pio_handler_set(BUT2_PIO,
                    BUT2_PIO_ID,
                    BUT2_PIO_IDX_MASK,
                    PIO_IT_FALL_EDGE,
                    but2_callback);
    pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);
    pio_get_interrupt_status(BUT2_PIO);
    NVIC_EnableIRQ(BUT2_PIO_ID);
    NVIC_SetPriority(BUT2_PIO_ID, 4);

    // led2
    pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_PIO_IDX_MASK, PIO_DEFAULT);
    pio_set(LED2_PIO, LED2_PIO_IDX_MASK);
}

// handlers ----
void RTT_Handler(void)
{
    uint32_t ul_status;
    ul_status = rtt_get_status(RTT);

    if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS)
    {
    }

    if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC)
    {
    }
}

// sw
void clear_screen()
{
    gfx_mono_generic_draw_filled_rect(0, 0, 67, 31, GFX_PIXEL_CLR);
    return;
}

void draw_string(char *text)
{
    clear_screen();
    gfx_mono_draw_string("            ", 0, 3, &sysfont);
    gfx_mono_draw_string(text, 0, 3, &sysfont);
}

void but2_callback(void)
{
    but2_flag = 1;
}
_Bool get_but2_flag(void)
{
    return but2_flag;
}
void set_but2_flag(_Bool val)
{
    but2_flag = val;
}

void pin_toggle(Pio *pio, uint32_t mask)
{
    pio_get_output_data_status(pio, mask) ? pio_clear(pio, mask) : pio_set(pio, mask);
}
