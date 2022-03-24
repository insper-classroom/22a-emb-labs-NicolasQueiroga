#include "asf.h"

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

void init(void);
void blink(Pio *, const uint32_t);

void blink(Pio *pio, const uint32_t mask)
{
	int cnt = -1;
	while (++cnt < 5)
	{
		pio_set(pio, mask);
		delay_ms(100);
		pio_clear(pio, mask);
		delay_ms(100);
	}
	pio_set(pio, mask);
}

void init(void)
{
	sysclk_init();
	WDT->WDT_MR = WDT_MR_WDDIS;

	pmc_enable_periph_clk(LED_PIO_ID);
	pmc_enable_periph_clk(LED1_PIO_ID);
	pmc_enable_periph_clk(LED2_PIO_ID);
	pmc_enable_periph_clk(LED3_PIO_ID);

	pmc_enable_periph_clk(BUT_PIO_ID);
	pmc_enable_periph_clk(BUT1_PIO_ID);
	pmc_enable_periph_clk(BUT2_PIO_ID);
	pmc_enable_periph_clk(BUT3_PIO_ID);

	pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);
	pio_set_output(LED1_PIO, LED1_PIO_IDX_MASK, 0, 0, 0);
	pio_set_output(LED2_PIO, LED2_PIO_IDX_MASK, 0, 0, 0);
	pio_set_output(LED3_PIO, LED3_PIO_IDX_MASK, 0, 0, 0);

	pio_set_input(BUT_PIO, BUT_PIO_IDX_MASK, PIO_DEFAULT);
	pio_set_input(BUT1_PIO, BUT1_PIO_IDX_MASK, PIO_DEFAULT);
	pio_set_input(BUT2_PIO, BUT2_PIO_IDX_MASK, PIO_DEFAULT);
	pio_set_input(BUT3_PIO, BUT3_PIO_IDX_MASK, PIO_DEFAULT);

	pio_pull_up(BUT_PIO, BUT_PIO_IDX_MASK, 1);
	pio_pull_up(BUT1_PIO, BUT1_PIO_IDX_MASK, 1);
	pio_pull_up(BUT2_PIO, BUT2_PIO_IDX_MASK, 1);
	pio_pull_up(BUT3_PIO, BUT3_PIO_IDX_MASK, 1);
}

int main(void)
{
	init();

	while (1)
	{
		_Bool btn = !pio_get(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK);
		_Bool btn1 = !pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK);
		_Bool btn2 = !pio_get(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK);
		_Bool btn3 = !pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK);

		if (btn)
			blink(LED_PIO, LED_PIO_IDX_MASK);
		else if (btn1)
			blink(LED1_PIO, LED1_PIO_IDX_MASK);
		else if (btn2)
			blink(LED2_PIO, LED2_PIO_IDX_MASK);
		else if (btn3)
			blink(LED3_PIO, LED3_PIO_IDX_MASK);
		else
		{
			pio_set(LED_PIO, LED_PIO_IDX_MASK);
			pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
			pio_set(LED2_PIO, LED2_PIO_IDX_MASK);
			pio_set(LED3_PIO, LED3_PIO_IDX_MASK);
		}
	}
	return 0;
}
