#include "auxiliary/aux_time/aux_time.h"
#include "auxiliary/aux_lvgl/aux_lvgl.h"
#include "aux_rtos.h"

// semaphores
volatile SemaphoreHandle_t xSemaphoreRTC;

// queues

// getters & setters
SemaphoreHandle_t get_xSemaphoreRTC(void)
{
  return xSemaphoreRTC;
}

// rtos functions
void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName)
{
  printf("stack overflow %u %s\r\n", pxTask, (portCHAR *)pcTaskName);
  while (1)
    ;
}
void vApplicationIdleHook(void) { pmc_sleep(SAM_PM_SMODE_SLEEP_WFI); }
void vApplicationTickHook(void) {}
void vApplicationMallocFailedHook(void) { configASSERT((volatile void *)NULL); }

// tasks
void task_lcd(void *pvParameters)
{
  int px, py;
  lv_termostato();

  while (1)
  {
    lv_tick_inc(50);
    lv_task_handler();
    vTaskDelay(50);
  }
}

void task_update_clk(void *pvParameters)
{
  // RTC
  Calendar rtc_initial = {2022, 3, 19, 12, 15, 45, 1};
  set_cal(rtc_initial);
  RTC_init(RTC, ID_RTC, rtc_initial, RTC_SR_SEC | RTC_SR_ALARM);
  uint32_t current_hour, current_min, current_sec;

  while (1)
  {
    rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
    if (xSemaphoreTake(xSemaphoreRTC, 1000 / portTICK_PERIOD_MS))
      lv_label_set_text_fmt(get_label_clock(), "%02d:%02d", current_hour, current_min);
    else
      lv_label_set_text_fmt(get_label_clock(), "%02d %02d", current_hour, current_min);
  }
}

// init rtos
void init_rtos(void)
{
  // create semaphores
  xSemaphoreRTC = xSemaphoreCreateBinary();

  // create tasks
  xTaskCreate(task_lcd, "lcd", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL);
  xTaskCreate(task_update_clk, "update_clk", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL);

  vTaskStartScheduler();
}