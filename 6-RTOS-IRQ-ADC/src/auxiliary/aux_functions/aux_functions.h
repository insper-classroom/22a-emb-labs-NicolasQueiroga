#ifndef __AUX_FUNCTIONS_H_
#define __AUX_FUNCTIONS_H_

#include "asf.h"

void clear_screen(void);
void draw_string(char *text);
void draw_time(int h, int m, int s);
void pin_toggle(Pio *pio, uint32_t mask);
void blink_led(Pio *p_pio, const uint32_t ul_mask, int n, int t);

#endif