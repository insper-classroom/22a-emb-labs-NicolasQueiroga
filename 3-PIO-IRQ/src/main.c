#include "asf.h"

#define LED_PIO PIOC
#define LED_PIO_ID ID_PIOC
#define LED_IDX 8
#define LED_IDX_MASK (1u << LED_IDX)

#define BUT_PIO PIOA
#define BUT_PIO_ID ID_PIOA
#define BUT_IDX 11
#define BUT_IDX_MASK (1u << BUT_IDX)

void io_init(void);
void pisca_led(int n, int t);

void but_callback(void)
{
  pisca_led(5, 20);
}

void pisca_led(int n, int t)
{
  for (int i = 0; i < n; i++)
  {
    pio_clear(LED_PIO, LED_IDX_MASK);
    delay_ms(t);
    pio_set(LED_PIO, LED_IDX_MASK);
    delay_ms(t);
  }
}

void io_init(void)
{
  pmc_enable_periph_clk(LED_PIO_ID);
  pmc_enable_periph_clk(BUT_PIO_ID);
  pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT);
  pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP);

  pio_handler_set(BUT_PIO,
                  BUT_PIO_ID,
                  BUT_IDX_MASK,
                  PIO_IT_RISE_EDGE,
                  but_callback);

  pio_enable_interrupt(BUT_PIO, BUT_IDX_MASK);
  pio_get_interrupt_status(BUT_PIO);

  NVIC_EnableIRQ(BUT_PIO_ID);
  NVIC_SetPriority(BUT_PIO_ID, 4);
}

void main(void)
{
  sysclk_init();
  WDT->WDT_MR = WDT_MR_WDDIS;
  io_init();
  while (1)
  {
  }
}
