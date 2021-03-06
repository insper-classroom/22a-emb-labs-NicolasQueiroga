#ifndef __AUX_RTOS_H_
#define __AUX_RTOS_H_

#include "auxiliary/aux_hw/aux_hw.h"

#define TASK_LCD_STACK_SIZE (1024 * 6 / sizeof(portSTACK_TYPE))
#define TASK_LCD_STACK_PRIORITY (tskIDLE_PRIORITY)

#define TASK_ADC_STACK_SIZE (1024 * 10 / sizeof(portSTACK_TYPE))
#define TASK_ADC_STACK_PRIORITY (tskIDLE_PRIORITY)

#define TASK_PROC_STACK_SIZE (1024 * 10 / sizeof(portSTACK_TYPE))
#define TASK_PROC_STACK_PRIORITY (tskIDLE_PRIORITY)

typedef struct
{
    uint value;
} adcData;

// getters & setters
SemaphoreHandle_t get_xSemaphoreRTC(void);
SemaphoreHandle_t get_xSemaphoreMutex(void);

// rtos functions
void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName);
void vApplicationIdleHook(void);
void vApplicationTickHook(void);
void vApplicationMallocFailedHook(void);
void xPortSysTickHandler(void);

// tasks
void task_lcd(void *pvParameters);
void task_update_clk(void *pvParameters);

// init rtos
void init_rtos(void);

#endif