#ifndef __AUXILIARY_H_
#define __AUXILIARY_H_

#include "asf.h"
#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

#define LED2_PIO PIOC
#define LED2_PIO_ID ID_PIOC
#define LED2_PIO_IDX 30
#define LED2_PIO_IDX_MASK (1u << LED2_PIO_IDX)

#define BUT2_PIO PIOC
#define BUT2_PIO_ID ID_PIOC
#define BUT2_PIO_IDX 31
#define BUT2_PIO_IDX_MASK (1u << BUT2_PIO_IDX)

// hw ----
// inits
void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
void init(void);

// handlers
void RTT_Handler(void);

// sw ----
void but2_callback(void);
_Bool get_but2_flag(void);
void set_but2_flag(_Bool val);

void clear_screen();
void draw_string(char *text);
void pin_toggle(Pio *pio, uint32_t mask);

#endif