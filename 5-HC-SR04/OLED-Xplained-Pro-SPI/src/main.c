#include "asf.h"
#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

#define BUT_PIO PIOA
#define BUT_PIO_ID ID_PIOA
#define BUT_PIO_IDX 11
#define BUT_PIO_IDX_MASK (1u << BUT_PIO_IDX)

#define TRIG_PIO PIOD
#define TRIG_PIO_ID ID_PIOD
#define TRIG_PIO_IDX 30
#define TRIG_PIO_IDX_MASK (1u << TRIG_PIO_IDX)

#define ECHO_PIO PIOA
#define ECHO_PIO_ID ID_PIOA
#define ECHO_PIO_IDX 6
#define ECHO_PIO_IDX_MASK (1u << ECHO_PIO_IDX)

#define FREQUENCY 1 / (2 * 0.000058)
#define SND_SPEED 340.0

volatile char but_flag = 0;
volatile char echo_flag = 0;
volatile uint32_t rtt_time = 0;

static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
void init(void);

void but_callback(void)
{
	but_flag = 1;
}

void echo_callback(void)
{
	if (pio_get(ECHO_PIO, PIO_INPUT, ECHO_PIO_IDX_MASK))
	{

		RTT_init(FREQUENCY, 0, 0);
		echo_flag = 1;
	}
	else
	{
		echo_flag = 0;
		rtt_time = rtt_read_timer_value(RTT);
	}
}

void pin_toggle(Pio *pio, uint32_t mask)
{
	pio_get_output_data_status(pio, mask) ? pio_clear(pio, mask) : pio_set(pio, mask);
}

void sensor_trigger()
{
	gfx_mono_draw_string("trig  ", 0, 16, &sysfont);
	pio_set(TRIG_PIO, TRIG_PIO_IDX_MASK);
	delay_us(10);
	pio_clear(TRIG_PIO, TRIG_PIO_IDX_MASK);
}

void draw_distance(float distance)
{
	char str[20];
	gfx_mono_draw_string("             ", 0, 16, &sysfont);
	sprintf(str, "%.1f cm", distance);
	gfx_mono_draw_string(str, 0, 16, &sysfont);
}

void RTT_Handler(void)
{
	uint32_t ul_status;
	ul_status = rtt_get_status(RTT);

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

	// echo
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
	NVIC_SetPriority(ECHO_PIO_ID, 4);

	// trigger
	pio_configure(TRIG_PIO, PIO_OUTPUT_0, TRIG_PIO_IDX_MASK, PIO_DEFAULT);
}

void main(void)
{
	init();
	float dist;
	gfx_mono_draw_string("init", 0, 16, &sysfont);

	while (1)
	{
		if (but_flag)
		{
			but_flag = 0;
			gfx_mono_draw_string("btn  ", 0, 16, &sysfont);
			sensor_trigger();
		}
		if (echo_flag)
		{

			dist = (SND_SPEED * rtt_time * 100) / (2 * FREQUENCY);
			if (dist > 400 || dist < 2)
			{
				gfx_mono_draw_string("ERRO", 0, 16, &sysfont);
				// deu ruim
			}
			else
			{
				draw_distance(dist);
				// desenha dist e grafico
			}
		}
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
}