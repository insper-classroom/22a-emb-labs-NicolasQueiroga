#ifndef __AUX_HW_H_
#define __AUX_HW_H_

#include "conf_board.h"
#include "asf.h"

#define USART_COM_ID ID_USART1
#define USART_COM USART1

#define AFEC_POT AFEC1
#define AFEC_POT_ID ID_AFEC1
#define AFEC_POT_CHANNEL 6 // Canal do pino PC31

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