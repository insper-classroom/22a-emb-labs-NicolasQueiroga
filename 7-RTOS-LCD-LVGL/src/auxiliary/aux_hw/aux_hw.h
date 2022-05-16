#ifndef __AUX_HW_H_
#define __AUX_HW_H_

#include "conf_board.h"
#include "asf.h"

// init basic
void init(void);

// callbacks
void but1_callback(void);
void but2_callback(void);
void but3_callback(void);
void AFEC_pot_callback(void);

// getters & setters

// configures
void configure_console(void);

#endif