#ifndef __AUX_TIME_H_
#define __AUX_TIME_H_

#include "asf.h"

typedef struct
{
    uint32_t year;
    uint32_t month;
    uint32_t day;
    uint32_t week;
    uint32_t hour;
    uint32_t minute;
    uint32_t second;
} Calendar;

// getters & setters
_Bool get_rtc_second_flag(void);
void set_rtc_second_flag(_Bool val);

_Bool get_rtc_alarm_flag(void);
void set_rtc_alarm_flag(_Bool val);

void set_cal(Calendar val);
Calendar get_cal(void);

// inits
void RTC_init(Rtc *rtc, uint32_t id_rtc, Calendar t, uint32_t irq_type);

// handlers
void RTC_Handler(void);

#endif