#include "auxiliary.h"

// globals
volatile _Bool but1_flag = 0;
volatile _Bool but2_flag = 0;
volatile _Bool but3_flag = 0;
volatile _Bool tc0_flag = 0;
volatile _Bool tc1_flag = 0;
volatile _Bool flag_rtc_second = 0;
volatile _Bool flag_rtc_alarm = 0;

// hw
// inits ----
void TC_init(Tc *TC, int ID_TC, int TC_CHANNEL, int freq)
{
    uint32_t ul_div;
    uint32_t ul_tcclks;
    uint32_t ul_sysclk = sysclk_get_cpu_hz();

    /* Configura o PMC */
    pmc_enable_periph_clk(ID_TC);

    /** Configura o TC para operar em  freq hz e interrupçcão no RC compare */
    tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
    tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
    tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

    /* Configura NVIC*/
    NVIC_SetPriority(ID_TC, 4);
    NVIC_EnableIRQ((IRQn_Type)ID_TC);
    tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);
}

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

void RTC_init(Rtc *rtc, uint32_t id_rtc, Calendar t, uint32_t irq_type)
{
    // PMC configure
    pmc_enable_periph_clk(ID_RTC);

    // default RTC configuration -> 24-hour mode
    rtc_set_hour_mode(rtc, 0);

    // set date and time manually
    rtc_set_date(rtc, t.year, t.month, t.day, t.week);
    rtc_set_time(rtc, t.hour, t.minute, t.second);

    // configure RTC interrupts
    NVIC_DisableIRQ(id_rtc);
    NVIC_ClearPendingIRQ(id_rtc);
    NVIC_SetPriority(id_rtc, 4);
    NVIC_EnableIRQ(id_rtc);

    // activates interruption via alarm
    rtc_enable_interrupt(rtc, irq_type);
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
    pmc_enable_periph_clk(ID_PIOA);
    pmc_enable_periph_clk(ID_PIOB);
    pmc_enable_periph_clk(ID_PIOC);
    pmc_enable_periph_clk(ID_PIOD);

    // btn1
    pio_configure(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
    pio_set_debounce_filter(BUT1_PIO, BUT1_PIO_IDX_MASK, 60);
    pio_handler_set(BUT1_PIO,
                    BUT1_PIO_ID,
                    BUT1_PIO_IDX_MASK,
                    PIO_IT_FALL_EDGE,
                    but1_callback);
    pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
    pio_get_interrupt_status(BUT1_PIO);
    NVIC_EnableIRQ(BUT1_PIO_ID);
    NVIC_SetPriority(BUT1_PIO_ID, 4);

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

    // btn3
    pio_configure(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
    pio_set_debounce_filter(BUT3_PIO, BUT3_PIO_IDX_MASK, 60);
    pio_handler_set(BUT3_PIO,
                    BUT3_PIO_ID,
                    BUT3_PIO_IDX_MASK,
                    PIO_IT_FALL_EDGE,
                    but3_callback);
    pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);
    pio_get_interrupt_status(BUT3_PIO);
    NVIC_EnableIRQ(BUT3_PIO_ID);
    NVIC_SetPriority(BUT3_PIO_ID, 4);

    // led
    pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_PIO_IDX_MASK, PIO_DEFAULT);
    pio_set(LED1_PIO, LED1_PIO_IDX_MASK);

    // led2
    pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_PIO_IDX_MASK, PIO_DEFAULT);
    pio_set(LED2_PIO, LED2_PIO_IDX_MASK);

    // led3
    pio_configure(LED3_PIO, PIO_OUTPUT_0, LED3_PIO_IDX_MASK, PIO_DEFAULT);
    pio_set(LED3_PIO, LED3_PIO_IDX_MASK);
}

// handlers ----
void TC0_Handler(void)
{
    volatile uint32_t status = tc_get_status(TC0, 0);
    tc0_flag = 1;
}

void TC1_Handler(void)
{
    volatile uint32_t status = tc_get_status(TC0, 1);
    pin_toggle(LED3_PIO, LED3_PIO_IDX_MASK);
    tc1_flag = 1;
}

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

void RTC_Handler(void)
{
    uint32_t ul_status = rtc_get_status(RTC);

    if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC)
        flag_rtc_second = 1;

    if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM)
        flag_rtc_alarm = 1;

    rtc_clear_status(RTC, RTC_SCCR_SECCLR);
    rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
    rtc_clear_status(RTC, RTC_SCCR_ACKCLR);
    rtc_clear_status(RTC, RTC_SCCR_TIMCLR);
    rtc_clear_status(RTC, RTC_SCCR_CALCLR);
    rtc_clear_status(RTC, RTC_SCCR_TDERRCLR);
}

// sw
void clear_screen()
{
    gfx_mono_generic_draw_filled_rect(0, 0, 127, 31, GFX_PIXEL_CLR);
    return;
}

void draw_string(char *text)
{
    gfx_mono_draw_string("            ", 0, 3, &sysfont);
    gfx_mono_draw_string(text, 0, 3, &sysfont);
}

void draw_dots(int x)
{
    gfx_mono_draw_filled_circle(x, 25, 2, GFX_PIXEL_SET, GFX_WHOLE);
}

void draw_time(int h, int m, int s)
{
    char freq_str[20];
    sprintf(freq_str, "%02d:%02d:%02d", h, m, s);
    gfx_mono_draw_string(freq_str, 0, 16, &sysfont);
}

void but1_callback(void)
{
    but1_flag = 1;
}
_Bool get_but1_flag(void)
{
    return but1_flag;
}
void set_but1_flag(_Bool val)
{
    but1_flag = val;
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

void but3_callback(void)
{
    but3_flag = 1;
}
_Bool get_but3_flag(void)
{
    return but3_flag;
}
void set_but3_flag(_Bool val)
{
    but3_flag = val;
}

_Bool get_tc0_flag(void)
{
    return tc0_flag;
}
void set_tc0_flag(_Bool val)
{
    tc0_flag = val;
}

_Bool get_tc1_flag(void)
{
    return tc1_flag;
}
void set_tc1_flag(_Bool val)
{
    tc1_flag = val;
}

_Bool get_rtc_second_flag(void)
{
    return flag_rtc_second;
}
void set_rtc_second_flag(_Bool val)
{
    flag_rtc_second = val;
}

_Bool get_rtc_alarm_flag(void)
{
    return flag_rtc_alarm;
}
void set_rtc_alarm_flag(_Bool val)
{
    flag_rtc_alarm = val;
}

void pin_toggle(Pio *pio, uint32_t mask)
{
    pio_get_output_data_status(pio, mask) ? pio_clear(pio, mask) : pio_set(pio, mask);
}

void blink_led(Pio *p_pio, const uint32_t ul_mask, int n, int t)
{
    for (int i = 1; i <= n; i++)
    {
        pio_clear(p_pio, ul_mask);
        delay_ms(t / 2);
        pio_set(p_pio, ul_mask);
        delay_ms(t / 2);
    }
}