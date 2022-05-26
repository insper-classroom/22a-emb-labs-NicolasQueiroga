#ifndef __AUX_RTOS_H_
#define __AUX_RTOS_H_

#include "auxiliary/aux_hw/aux_hw.h"

#define TASK_ADC_STACK_SIZE (1024 * 10 / sizeof(portSTACK_TYPE))
#define TASK_ADC_STACK_PRIORITY (tskIDLE_PRIORITY)

#define TASK_PROC_STACK_SIZE (1024 * 10 / sizeof(portSTACK_TYPE))
#define TASK_PROC_STACK_PRIORITY (tskIDLE_PRIORITY)


// getters & setters
QueueHandle_t get_adc_queue(void);
void set_adc_queue(QueueHandle_t val);
QueueHandle_t get_proc_queue(void);
void set_proc_queue(QueueHandle_t val);

// rtos functions
void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName);
void vApplicationIdleHook(void);
void vApplicationTickHook(void);
void vApplicationMallocFailedHook(void);
void xPortSysTickHandler(void);

// tasks
void task_proc(void *pvParameters);
void task_adc(void *pvParameters);

#endif