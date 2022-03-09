#include <asf.h>

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

volatile char but_flag = 0;
volatile char but1_flag = 0;
volatile char but2_flag = 0;
volatile char but3_flag = 0;

volatile int frequencia = 500;

void init(void);
void but_callback(void);
void but1_callback(void);
void but2_callback(void);
void but3_callback(void);
void get_frequency();
void draw_frequency(int frequencia);
void pisca_led(int n, int t);

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

void get_frequency()
{
	for (int i = 0; i < 10000000; i++)
	{
		if (i >= 1000000 && !pio_get(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK))
		{
			frequencia += 100;
			delay_ms(300);
			draw_frequency(frequencia);
			break;
		}
		else if (i < 1000000 && !pio_get(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK))
		{
			frequencia -= 100;
			delay_ms(300);
			draw_frequency(frequencia);
			break;
		}
	}
}

void draw_frequency(int frequencia)
{
	char freq_str[20];
	sprintf(freq_str, "%d ms       ", frequencia);
	gfx_mono_draw_string(freq_str, 0, 16, &sysfont);
}

void pisca_led(int n, int t)
{
	gfx_mono_generic_draw_filled_rect(81, 17, 31, 9, GFX_PIXEL_CLR);
	gfx_mono_generic_draw_rect(80, 16, 32, 10, GFX_PIXEL_SET);
	for (int i = 1; i <= n; i++)
	{
		gfx_mono_generic_draw_vertical_line(80 + i, 16, 10, GFX_PIXEL_SET);
		pio_clear(LED2_PIO, LED2_PIO_IDX_MASK);
		delay_ms(t / 2);
		pio_set(LED2_PIO, LED2_PIO_IDX_MASK);
		delay_ms(t / 2);
		if (but2_flag)
		{
			delay_ms(200);
			break;
		}
	}
}

void init(void)
{
	// setup
	board_init();
	sysclk_init();
	delay_init();
	gfx_mono_ssd1306_init();
	WDT->WDT_MR = WDT_MR_WDDIS;

	// btn
	pmc_enable_periph_clk(BUT_PIO_ID);
	pio_configure(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_PIO, BUT_PIO_IDX_MASK, 60);
	pio_handler_set(BUT_PIO,
					BUT_PIO_ID,
					BUT_PIO_IDX_MASK,
					PIO_IT_EDGE,
					but_callback);
	pio_enable_interrupt(BUT_PIO, BUT_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT_PIO);
	NVIC_EnableIRQ(BUT_PIO_ID);
	NVIC_SetPriority(BUT_PIO_ID, 4);

	// btn2
	pmc_enable_periph_clk(BUT2_PIO_ID);
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

	// // btn3
	// pmc_enable_periph_clk(BUT3_PIO_ID);
	// pio_configure(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	// pio_set_debounce_filter(BUT3_PIO, BUT3_PIO_IDX_MASK, 60);
	// pio_handler_set(BUT3_PIO,
	// 				BUT3_PIO_ID,
	// 				BUT3_PIO_IDX_MASK,
	// 				PIO_IT_FALL_EDGE,
	// 				but3_callback);
	// pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);
	// pio_get_interrupt_status(BUT3_PIO);
	// NVIC_EnableIRQ(BUT3_PIO_ID);
	// NVIC_SetPriority(BUT3_PIO_ID, 4);

	// led2
	pmc_enable_periph_clk(LED2_PIO_ID);
	pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_PIO_IDX_MASK, PIO_DEFAULT);
}

void main(void)
{
	init();
	gfx_mono_draw_string("     LAB 3", 0, 16, &sysfont);

	while (1)
	{
		if (but_flag || but2_flag)
		{
			if (pio_get(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK))
				get_frequency();
			else
			{
				delay_ms(200);
				but2_flag = 0;
				pisca_led(30, frequencia);
			}
			draw_frequency(frequencia);
			but_flag = 0;
			but2_flag = 0;
		}
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}
}
