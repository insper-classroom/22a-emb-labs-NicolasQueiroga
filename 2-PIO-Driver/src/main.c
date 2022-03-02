#include "asf.h"

#define LED_PIO PIOC
#define LED_PIO_ID ID_PIOC
#define LED_PIO_IDX 8
#define LED_PIO_IDX_MASK (1 << LED_PIO_IDX)

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

void _pio_set(Pio *, const uint32_t);
void _pio_clear(Pio *, const uint32_t);
void _pio_pull_up(Pio *, const uint32_t, const uint32_t);
void _pio_set_input(Pio *, const uint32_t, const uint32_t);
void _pio_set_output(Pio *, const uint32_t, const uint32_t, const uint32_t, const uint32_t);
void _delay_ms(int);
uint32_t _pio_get(Pio *, const pio_type_t, const uint32_t);
void init(void);
void blink(Pio *, const uint32_t);

void _pio_set(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_SODR = ul_mask;
}

void _pio_clear(Pio *p_pio, const uint32_t ul_mask)
{
	p_pio->PIO_CODR = ul_mask;
}

void _pio_pull_up(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_pull_up_enable)
{
	if (ul_pull_up_enable)
		p_pio->PIO_PUER = ul_mask;
	else
		p_pio->PIO_PUDR = ul_mask;
}

void _pio_set_input(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_attribute)
{
	_pio_pull_up(p_pio, ul_mask, ul_attribute & PIO_PULLUP);

	if (ul_attribute & (PIO_DEGLITCH | PIO_DEBOUNCE))
		p_pio->PIO_IFER = ul_mask;
	else
		p_pio->PIO_IFDR = ul_mask;

	p_pio->PIO_ODR = ul_mask;
	p_pio->PIO_PER = ul_mask;
}

void _pio_set_output(Pio *p_pio, const uint32_t ul_mask, const uint32_t ul_default_level, const uint32_t ul_multidrive_enable, const uint32_t ul_pull_up_enable)
{
	_pio_pull_up(p_pio, ul_mask, ul_pull_up_enable);

	if (ul_multidrive_enable)
		p_pio->PIO_MDER = ul_mask;
	else
		p_pio->PIO_MDDR = ul_mask;

	if (ul_default_level)
		_pio_set(p_pio, ul_mask);
	else
		_pio_clear(p_pio, ul_mask);

	p_pio->PIO_OER = ul_mask;
	p_pio->PIO_PER = ul_mask;
}

uint32_t _pio_get(Pio *p_pio, const pio_type_t ul_type, const uint32_t ul_mask)
{
	uint32_t ul_reg;

	if ((ul_type == PIO_OUTPUT_0) || (ul_type == PIO_OUTPUT_1))
		ul_reg = p_pio->PIO_ODSR;
	else
		ul_reg = p_pio->PIO_PDSR;

	if ((ul_reg & ul_mask) == 0)
		return 0;
	else
		return 1;
}

void _delay_ms(int ms)
{
	for (int i = 0; i < (ms * 150000); i++)
		asm("NOP");
}

void blink(Pio *pio, const uint32_t mask)
{
	int cnt = -1;
	while (++cnt < 5)
	{
		_pio_set(pio, mask);
		_delay_ms(100);
		_pio_clear(pio, mask);
		_delay_ms(100);
	}
	_pio_set(pio, mask);
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

	_pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 0, 0, 0);
	_pio_set_output(LED1_PIO, LED1_PIO_IDX_MASK, 0, 0, 0);
	_pio_set_output(LED2_PIO, LED2_PIO_IDX_MASK, 0, 0, 0);
	_pio_set_output(LED3_PIO, LED3_PIO_IDX_MASK, 0, 0, 0);

	_pio_set_input(BUT_PIO, BUT_PIO_IDX_MASK, PIO_DEFAULT);
	_pio_set_input(BUT1_PIO, BUT1_PIO_IDX_MASK, PIO_DEFAULT);
	_pio_set_input(BUT2_PIO, BUT2_PIO_IDX_MASK, PIO_DEFAULT);
	_pio_set_input(BUT3_PIO, BUT3_PIO_IDX_MASK, PIO_DEFAULT);

	_pio_pull_up(BUT_PIO, BUT_PIO_IDX_MASK, 1);
	_pio_pull_up(BUT1_PIO, BUT1_PIO_IDX_MASK, 1);
	_pio_pull_up(BUT2_PIO, BUT2_PIO_IDX_MASK, 1);
	_pio_pull_up(BUT3_PIO, BUT3_PIO_IDX_MASK, 1);
}

int main(void)
{
	init();
	while (1)
	{
		_Bool btn = !_pio_get(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK);
		_Bool btn1 = !_pio_get(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK);
		_Bool btn2 = !_pio_get(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK);
		_Bool btn3 = !_pio_get(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK);

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
			_pio_set(LED_PIO, LED_PIO_IDX_MASK);
			_pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
			_pio_set(LED2_PIO, LED2_PIO_IDX_MASK);
			_pio_set(LED3_PIO, LED3_PIO_IDX_MASK);
		}
	}
	return 0;
}
