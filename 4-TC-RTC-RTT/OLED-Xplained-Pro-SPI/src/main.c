#include "asf.h"
#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

#define LED_PIO PIOC
#define LED_PIO_ID ID_PIOC
#define LED_PIO_IDX 8
#define LED_PIO_IDX_MASK (1u << LED_PIO_IDX)

#define BUT_PIO PIOA
#define BUT_PIO_ID ID_PIOA
#define BUT_PIO_IDX 11
#define BUT_PIO_IDX_MASK (1u << BUT_PIO_IDX)

#define LED1_PIO PIOA
#define LED1_PIO_ID ID_PIOA
#define LED1_PIO_IDX 0
#define LED1_PIO_IDX_MASK (1u << LED1_PIO_IDX)

#define LED2_PIO PIOC
#define LED2_PIO_ID ID_PIOC
#define LED2_PIO_IDX 30
#define LED2_PIO_IDX_MASK (1u << LED2_PIO_IDX)

#define LED3_PIO PIOB
#define LED3_PIO_ID ID_PIOB
#define LED3_PIO_IDX 2
#define LED3_PIO_IDX_MASK (1u << LED3_PIO_IDX)

#define BUT1_PIO PIOD
#define BUT1_PIO_ID ID_PIOD
#define BUT1_PIO_IDX 28
#define BUT1_PIO_IDX_MASK (1u << BUT1_PIO_IDX)

#define BUT2_PIO PIOC
#define BUT2_PIO_ID ID_PIOC
#define BUT2_PIO_IDX 31
#define BUT2_PIO_IDX_MASK (1u << BUT2_PIO_IDX)

#define BUT3_PIO PIOA
#define BUT3_PIO_ID ID_PIOA
#define BUT3_PIO_IDX 19
#define BUT3_PIO_IDX_MASK (1u << BUT3_PIO_IDX)

typedef struct
{
	uint32_t year;
	uint32_t month;
	uint32_t day;
	uint32_t week;
	uint32_t hour;
	uint32_t minute;
	uint32_t second;
} Calendar;

volatile char but_flag = 0;
volatile char but1_flag = 0;
volatile char but2_flag = 0;
volatile char but3_flag = 0;
volatile char flag_rtc_alarm = 0;
volatile char flag_rtc_second = 0;

void but_callback(void);
void but1_callback(void);
void but2_callback(void);
void but3_callback(void);
void pisca_led(Pio *p_pio, const uint32_t ul_mask, int n, int t);
void TC_init(Tc *TC, int ID_TC, int TC_CHANNEL, int freq);
void TC0_Handler(void);
void TC1_Handler(void);
void TC2_Handler(void);
static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
void RTT_Handler(void);
void RTC_init(Rtc *rtc, uint32_t id_rtc, Calendar t, uint32_t irq_type);
void RTC_Handler(void);
void init(void);

void but_callback(void)
{
	but_flag = 1;
}

void but1_callback(void)
{
	but1_flag = 1;
}

void but2_callback(void)
{
	but2_flag = 1;
}

void but3_callback(void)
{
	but3_flag = 1;
}

void pisca_led(Pio *p_pio, const uint32_t ul_mask, int n, int t)
{
	for (int i = 1; i <= n; i++)
	{
		pio_clear(p_pio, ul_mask);
		delay_ms(t / 2);
		pio_set(p_pio, ul_mask);
		delay_ms(t / 2);
	}
}

void pin_toggle(Pio *pio, uint32_t mask)
{
	if (pio_get_output_data_status(pio, mask))
		pio_clear(pio, mask);
	else
		pio_set(pio, mask);
}

void TC0_Handler(void)
{
	volatile uint32_t status = tc_get_status(TC0, 0);
	pin_toggle(LED3_PIO, LED3_PIO_IDX_MASK);
}

void TC1_Handler(void)
{
	volatile uint32_t status = tc_get_status(TC0, 1);
	pin_toggle(LED1_PIO, LED1_PIO_IDX_MASK);
}

void TC2_Handler(void)
{
	volatile uint32_t status = tc_get_status(TC0, 2);
	pin_toggle(LED_PIO, LED_PIO_IDX_MASK);
}

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

void RTT_Handler(void)
{
	uint32_t ul_status;
	ul_status = rtt_get_status(RTT);

	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS)
	{
		RTT_init(4, 0, RTT_MR_RTTINCIEN);
		pin_toggle(LED2_PIO, LED2_PIO_IDX_MASK);
	}

	if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC)
	{
	}
}

static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource)
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

	// btn
	pio_configure(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_PIO, BUT_PIO_IDX_MASK, 60);
	pio_handler_set(BUT_PIO,
					BUT_PIO_ID,
					BUT_PIO_IDX_MASK,
					PIO_IT_FALL_EDGE,
					but_callback);
	pio_enable_interrupt(BUT_PIO, BUT_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT_PIO);
	NVIC_EnableIRQ(BUT_PIO_ID);
	NVIC_SetPriority(BUT_PIO_ID, 4);

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
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_PIO_IDX_MASK, PIO_DEFAULT);
	pio_set(LED_PIO, LED_PIO_IDX_MASK);

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

void draw_time(int h, int m, int s)
{
	char freq_str[20];
	sprintf(freq_str, "%02d:%02d:%02d", h, m, s);
	gfx_mono_draw_string(freq_str, 0, 16, &sysfont);
}

void main(void)
{
	init();

	// TC
	TC_init(TC0, ID_TC1, 1, 5);
	tc_start(TC0, 1);

	TC_init(TC0, ID_TC2, 2, 4);
	tc_start(TC0, 2);
	// ---

	// RTT
	RTT_init(4, 16, RTT_MR_ALMIEN);
	// ---

	// RTC
	Calendar rtc_initial = {2018, 3, 19, 12, 15, 45, 1};
	RTC_init(RTC, ID_RTC, rtc_initial, RTC_SR_SEC | RTC_SR_ALARM);

	uint32_t current_hour, current_min, current_sec;
	uint32_t current_year, current_month, current_day, current_week;
	rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
	rtc_get_date(RTC, &current_year, &current_month, &current_day, &current_week);
	// ---

	while (1)
	{
		if (flag_rtc_second)
		{
			rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
			draw_time(current_hour, current_min, current_sec);
		}
		if (but1_flag)
		{
			but1_flag = 0;
			uint32_t next_min, next_sec;

			next_sec = current_sec + 20;

			if (next_sec >= 60)
			{
				next_min = current_min + 1;
				next_sec = next_sec - 60;
			}
			else
				next_min = current_min;

			rtc_set_time_alarm(RTC, 1, current_hour, 1, next_min, 1, next_sec);
		}
		if (flag_rtc_alarm)
		{
			flag_rtc_alarm = 0;
			TC_init(TC0, ID_TC0, 0, 4);
			tc_start(TC0, 0);
		}
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
}