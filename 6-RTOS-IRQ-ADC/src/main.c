#include "auxiliary/aux_hw/aux_hw.h"
#include "auxiliary/aux_time/aux_time.h"
#include "auxiliary/aux_rtos/aux_rtos.h"
#include "auxiliary/aux_bt/aux_bt.h"
#include "auxiliary/aux_functions/aux_functions.h"

void test_time(uint32_t current_hour, uint32_t current_min, uint32_t current_sec)
{
	if (get_rtc_second_flag())
	{
		rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
		draw_time(current_hour, current_min, current_sec);
	}
	if (get_tc0_flag())
	{
		set_tc0_flag(0);
		pin_toggle(LED2_PIO, LED2_PIO_IDX_MASK);
	}

	if (get_but1_flag())
	{
		set_but1_flag(0);
		pio_clear(LED1_PIO, LED1_PIO_IDX_MASK);
		RTT_init(4, 16, RTT_MR_ALMIEN);
	}
}

void test_rtos()
{
	set_adc_queue(xQueueCreate(100, sizeof(int)));
	if (xTaskCreate(task_adc, "ADC", TASK_ADC_STACK_SIZE, NULL, TASK_ADC_STACK_PRIORITY, NULL) != pdPASS)
		printf("Failed to create test ADC task\r\n");

	set_proc_queue(xQueueCreate(100, sizeof(adcData)));
	if (xTaskCreate(task_proc, "PROC", TASK_ADC_STACK_SIZE, NULL, TASK_ADC_STACK_PRIORITY, NULL) != pdPASS)
		printf("Failed to create test PROC task\r\n");
	/* Start the scheduler. */
	vTaskStartScheduler();
}

int main(void)
{
	init();

	// TC_init(TC0, ID_TC0, 0, 10);
	// tc_start(TC0, 0);

	// Calendar rtc_initial = {2018, 3, 19, 12, 15, 45, 1};
	// RTC_init(RTC, ID_RTC, rtc_initial, RTC_SR_SEC | RTC_SR_ALARM);

	// uint32_t current_hour, current_min, current_sec;
	// uint32_t current_year, current_month, current_day, current_week;
	// rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
	// rtc_get_date(RTC, &current_year, &current_month, &current_day, &current_week);

	// test_rtos();

	set_adc_queue(xQueueCreate(100, sizeof(int)));
	if (xTaskCreate(task_adc, "ADC", TASK_ADC_STACK_SIZE, NULL, TASK_ADC_STACK_PRIORITY, NULL) != pdPASS)
		printf("Failed to create test ADC task\r\n");

	set_proc_queue(xQueueCreate(100, sizeof(adcData)));
	if (xTaskCreate(task_proc, "PROC", TASK_ADC_STACK_SIZE, NULL, TASK_ADC_STACK_PRIORITY, NULL) != pdPASS)
		printf("Failed to create test PROC task\r\n");

	vTaskStartScheduler();

	while (1)
	{
		// test_time(current_hour, current_min, current_sec);
		// pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
	}

	return 0;
}