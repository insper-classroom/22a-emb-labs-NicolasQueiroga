#include "aux_hw.h"
#include "auxiliary/aux_lvgl/aux_lvgl.h"

// globals

// init basic
void init(void)
{
	// setup
	board_init();
	sysclk_init();
	// configure_console();
	configure_lcd();
	configure_touch();
	configure_lvgl();
}

// configure uart
// void configure_console(void)
// {
// 	const usart_serial_options_t uart_serial_options = {
// 		.baudrate = CONF_UART_BAUDRATE,
// 		.charlength = CONF_UART_CHAR_LENGTH,
// 		.paritytype = CONF_UART_PARITY,
// 		.stopbits = CONF_UART_STOP_BITS,
// 	};

// 	/* Configure console UART. */
// 	stdio_serial_init(CONF_UART, &uart_serial_options);

// 	/* Specify that stdout should not be buffered. */
// 	setbuf(stdout, NULL);
// }