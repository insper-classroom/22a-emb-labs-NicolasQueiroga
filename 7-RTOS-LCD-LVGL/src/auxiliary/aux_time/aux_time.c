#include "aux_time.h"
#include "auxiliary/aux_rtos/aux_rtos.h"

// globals
volatile _Bool flag_rtc_second = 0;
volatile _Bool flag_rtc_alarm = 0;

// getters & setters
_Bool get_rtc_second_flag(void)
{
    return flag_rtc_second;
}
void set_rtc_second_flag(_Bool val)
{
    flag_rtc_second = val;
}

_Bool get_rtc_alarm_flag(void)
{
    return flag_rtc_alarm;
}
void set_rtc_alarm_flag(_Bool val)
{
    flag_rtc_alarm = val;
}

// inits
void RTC_init(Rtc *rtc, uint32_t id_rtc, Calendar t, uint32_t irq_type)
{
    // PMC configure
    pmc_enable_periph_clk(ID_RTC);

    // default RTC configuration -> 24-hour mode
    rtc_set_hour_mode(rtc, 0);

    // set date and time manually
    rtc_set_date(rtc, t.year, t.month, t.day, t.week);
    rtc_set_time(rtc, t.hour, t.minute, t.second);

    // configure RTC interrupts
    NVIC_DisableIRQ(id_rtc);
    NVIC_ClearPendingIRQ(id_rtc);
    NVIC_SetPriority(id_rtc, 4);
    NVIC_EnableIRQ(id_rtc);

    // activates interruption via alarm
    rtc_enable_interrupt(rtc, irq_type);
}

// handlers
void RTC_Handler(void)
{
    uint32_t ul_status = rtc_get_status(RTC);

    if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(get_xSemaphoreRTC(), &xHigherPriorityTaskWoken);
    }

    if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM)
        flag_rtc_alarm = 1;

    rtc_clear_status(RTC, RTC_SCCR_SECCLR);
    rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
    rtc_clear_status(RTC, RTC_SCCR_ACKCLR);
    rtc_clear_status(RTC, RTC_SCCR_TIMCLR);
    rtc_clear_status(RTC, RTC_SCCR_CALCLR);
    rtc_clear_status(RTC, RTC_SCCR_TDERRCLR);
}
