#ifndef __LAB5_H_
#define __LAB5_H_

#include "auxiliary/auxiliary.h"

#define TRIG_PIO PIOA
#define TRIG_PIO_ID ID_PIOA
#define TRIG_PIO_IDX 24
#define TRIG_PIO_IDX_MASK (1u << TRIG_PIO_IDX)

#define ECHO_PIO PIOD
#define ECHO_PIO_ID ID_PIOD
#define ECHO_PIO_IDX 26
#define ECHO_PIO_IDX_MASK (1u << ECHO_PIO_IDX)

#define FREQUENCY 1 / (2 * 0.000058)
#define SND_SPEED 340.0

// hw
void init_sensor(void);
void sensor_trigger();
void echo_callback(void);

// sw
_Bool get_echo_flag(void);
void set_echo_flag(_Bool val);

_Bool get_contact_error_flag(void);
void set_contact_error_flag(_Bool val);

uint32_t get_rtt_time(void);
void set_rtt_time(uint32_t val);

void draw_points_and_dist(float distance, int *x_cnt);
void draw_error(char type);

#endif