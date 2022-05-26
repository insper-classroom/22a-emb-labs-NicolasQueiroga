#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"
#include "aux_functions.h"

void clear_screen(void)
{
    gfx_mono_generic_draw_filled_rect(0, 0, 127, 31, GFX_PIXEL_CLR);
}

void draw_string(char *text)
{
    gfx_mono_draw_string("            ", 0, 3, &sysfont);
    gfx_mono_draw_string(text, 0, 3, &sysfont);
}

void draw_time(int h, int m, int s)
{
    char freq_str[20];
    sprintf(freq_str, "%02d:%02d:%02d", h, m, s);
    gfx_mono_draw_string(freq_str, 0, 16, &sysfont);
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