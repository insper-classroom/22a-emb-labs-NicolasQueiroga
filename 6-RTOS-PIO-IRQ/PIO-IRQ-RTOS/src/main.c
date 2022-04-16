#include "conf_board.h"
#include <asf.h>

/************************************************************************/
/* BOARD CONFIG                                                         */
/************************************************************************/

#define BUT_PIO PIOA
#define BUT_PIO_ID ID_PIOA
#define BUT_PIO_PIN 11
#define BUT_PIO_PIN_MASK (1 << BUT_PIO_PIN)

#define LED_PIO PIOC
#define LED_PIO_ID ID_PIOC
#define LED_PIO_IDX 8
#define LED_IDX_MASK (1 << LED_PIO_IDX)

#define LED1_PIO PIOA
#define LED1_PIO_ID ID_PIOA
#define LED1_PIO_IDX 0
#define LED1_PIO_IDX_MASK (1u << LED1_PIO_IDX)

#define LED2_PIO PIOC
#define LED2_PIO_ID ID_PIOC
#define LED2_PIO_IDX 30
#define LED2_PIO_IDX_MASK (1u << LED2_PIO_IDX)

#define LED3_PIO PIOB
#define LED3_PIO_ID ID_PIOB
#define LED3_PIO_IDX 2
#define LED3_PIO_IDX_MASK (1u << LED3_PIO_IDX)

#define BUT1_PIO PIOD
#define BUT1_PIO_ID ID_PIOD
#define BUT1_PIO_IDX 28
#define BUT1_PIO_IDX_MASK (1u << BUT1_PIO_IDX)

#define BUT2_PIO PIOC
#define BUT2_PIO_ID ID_PIOC
#define BUT2_PIO_IDX 31
#define BUT2_PIO_IDX_MASK (1u << BUT2_PIO_IDX)

#define BUT3_PIO PIOA
#define BUT3_PIO_ID ID_PIOA
#define BUT3_PIO_IDX 19
#define BUT3_PIO_IDX_MASK (1u << BUT3_PIO_IDX)

#define USART_COM_ID ID_USART1
#define USART_COM USART1

/************************************************************************/
/* RTOS                                                                */
/************************************************************************/

#define TASK_LED_STACK_SIZE (1025 / sizeof(portSTACK_TYPE))
#define TASK_LED_STACK_PRIORITY (tskIDLE_PRIORITY)
#define TASK_BUT_STACK_SIZE (2048 / sizeof(portSTACK_TYPE))
#define TASK_BUT_STACK_PRIORITY (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
                                          signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/************************************************************************/
/* recursos RTOS                                                        */
/************************************************************************/

/** Semaforo a ser usado pela task led */
SemaphoreHandle_t xSemaphoreBut;
SemaphoreHandle_t xSemaphoreBut1;
SemaphoreHandle_t xSemaphoreBut2;
SemaphoreHandle_t xSemaphoreBut3;

/** Queue for msg log send data */
QueueHandle_t xQueueLedFreq, xQueueBut;

/************************************************************************/
/* prototypes local                                                     */
/************************************************************************/

void but_callback(void);
static void BUT_init(void);
void pin_toggle(Pio *pio, uint32_t mask);
static void USART1_init(void);
void LED_init(int estado);

/************************************************************************/
/* RTOS application funcs                                               */
/************************************************************************/

/**
 * \brief Called if stack overflow during execution
 */
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
                                          signed char *pcTaskName)
{
  printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
  /* If the parameters have been corrupted then inspect pxCurrentTCB to
   * identify which task has overflowed its stack.
   */
  for (;;)
  {
  }
}

/**
 * \brief This function is called by FreeRTOS idle task
 */
extern void vApplicationIdleHook(void) { pmc_sleep(SAM_PM_SMODE_SLEEP_WFI); }

/**
 * \brief This function is called by FreeRTOS each tick
 */
extern void vApplicationTickHook(void) {}

extern void vApplicationMallocFailedHook(void)
{
  /* Called if a call to pvPortMalloc() fails because there is insufficient
  free memory available in the FreeRTOS heap.  pvPortMalloc() is called
  internally by FreeRTOS API functions that create tasks, queues, software
  timers, and semaphores.  The size of the FreeRTOS heap is set by the
  configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

  /* Force an assert. */
  configASSERT((volatile void *)NULL);
}

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/

void but_callback(void)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(xSemaphoreBut, &xHigherPriorityTaskWoken);
}

void but1_callback(void)
{
  char id = '1';
  xQueueSendFromISR(xQueueBut, &id, 0);
}

void but2_callback(void)
{
  char id = '2';
  xQueueSendFromISR(xQueueBut, &id, 0);
}

void but3_callback(void)
{
  char id = '3';
  xQueueSendFromISR(xQueueBut, &id, 0);
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_led(void *pvParameters)
{

  LED_init(1);

  uint32_t msg = 0;
  uint32_t delayMs = 2000;

  /* tarefas de um RTOS n�o devem retornar */
  for (;;)
  {
    /* verifica se chegou algum dado na queue, e espera por 0 ticks */
    if (xQueueReceive(xQueueLedFreq, &msg, (TickType_t)0))
    {
      /* chegou novo valor, atualiza delay ! */
      /* aqui eu poderia verificar se msg faz sentido (se esta no range certo)
       */
      /* converte ms -> ticks */
      delayMs = msg / portTICK_PERIOD_MS;
      printf("delay = %d \n", delayMs);
    }

    /* pisca LED */
    pin_toggle(LED_PIO, LED_IDX_MASK);

    /* suspende por delayMs */
    vTaskDelay(delayMs);
  }
}

static void task_but(void *pvParameters)
{

  uint32_t delayTicks = 2000;
  char queue = 0;

  xQueueBut = xQueueCreate(32, sizeof(char));
  if (xQueueBut == NULL)
    printf("falha em criar a queue \n");

  for (;;)
  {
    if (xQueueReceive(xQueueBut, &queue, (TickType_t)500))
    {
      printf("but%c\n", queue);
      if (queue == '1')
        delayTicks -= 100;
      else if (queue == '3')
        delayTicks += 100;

      xQueueSend(xQueueLedFreq, (void *)&delayTicks, 10);

      if (delayTicks <= 100)
        delayTicks = 900;
      else if (delayTicks >= 3000)
        delayTicks = 2000;
    }
  }
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

/**
 * \brief Configure the console UART.
 */
static void configure_console(void)
{
  const usart_serial_options_t uart_serial_options = {
      .baudrate = CONF_UART_BAUDRATE,
      .charlength = CONF_UART_CHAR_LENGTH,
      .paritytype = CONF_UART_PARITY,
      .stopbits = CONF_UART_STOP_BITS,
  };

  /* Configure console UART. */
  stdio_serial_init(CONF_UART, &uart_serial_options);

  /* Specify that stdout should not be buffered. */
  setbuf(stdout, NULL);
}

void pin_toggle(Pio *pio, uint32_t mask)
{
  if (pio_get_output_data_status(pio, mask))
    pio_clear(pio, mask);
  else
    pio_set(pio, mask);
}

void LED_init(int estado)
{
  pmc_enable_periph_clk(LED_PIO_ID);
  pio_set_output(LED_PIO, LED_IDX_MASK, estado, 0, 0);
};

void init(void)
{
  // setup
  board_init();
  sysclk_init();
  configure_console();

  // init clocks
  pmc_enable_periph_clk(ID_PIOA);
  pmc_enable_periph_clk(ID_PIOB);
  pmc_enable_periph_clk(ID_PIOC);
  pmc_enable_periph_clk(ID_PIOD);

  // btn1
  pio_configure(BUT1_PIO, PIO_INPUT, BUT1_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_set_debounce_filter(BUT1_PIO, BUT1_PIO_IDX_MASK, 60);
  pio_handler_set(BUT1_PIO,
                  BUT1_PIO_ID,
                  BUT1_PIO_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but1_callback);
  pio_enable_interrupt(BUT1_PIO, BUT1_PIO_IDX_MASK);
  pio_get_interrupt_status(BUT1_PIO);
  NVIC_EnableIRQ(BUT1_PIO_ID);
  NVIC_SetPriority(BUT1_PIO_ID, 5);

  // btn2
  pio_configure(BUT2_PIO, PIO_INPUT, BUT2_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_set_debounce_filter(BUT2_PIO, BUT2_PIO_IDX_MASK, 60);
  pio_handler_set(BUT2_PIO,
                  BUT2_PIO_ID,
                  BUT2_PIO_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but2_callback);
  pio_enable_interrupt(BUT2_PIO, BUT2_PIO_IDX_MASK);
  pio_get_interrupt_status(BUT2_PIO);
  NVIC_EnableIRQ(BUT2_PIO_ID);
  NVIC_SetPriority(BUT2_PIO_ID, 5);

  // btn3
  pio_configure(BUT3_PIO, PIO_INPUT, BUT3_PIO_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
  pio_set_debounce_filter(BUT3_PIO, BUT3_PIO_IDX_MASK, 60);
  pio_handler_set(BUT3_PIO,
                  BUT3_PIO_ID,
                  BUT3_PIO_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but3_callback);
  pio_enable_interrupt(BUT3_PIO, BUT3_PIO_IDX_MASK);
  pio_get_interrupt_status(BUT3_PIO);
  NVIC_EnableIRQ(BUT3_PIO_ID);
  NVIC_SetPriority(BUT3_PIO_ID, 5);

  // led
  pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_PIO_IDX_MASK, PIO_DEFAULT);
  pio_set(LED1_PIO, LED1_PIO_IDX_MASK);

  // led2
  pio_configure(LED2_PIO, PIO_OUTPUT_0, LED2_PIO_IDX_MASK, PIO_DEFAULT);
  pio_set(LED2_PIO, LED2_PIO_IDX_MASK);

  // led3
  pio_configure(LED3_PIO, PIO_OUTPUT_0, LED3_PIO_IDX_MASK, PIO_DEFAULT);
  pio_set(LED3_PIO, LED3_PIO_IDX_MASK);
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/

/**
 *  \brief FreeRTOS Real Time Kernel example entry point.
 *
 *  \return Unused (ANSI-C compatibility).
 */
int main(void)
{
  /* Initialize the SAM system */
  init();

  /* Attempt to create a semaphore. */
  xQueueBut = xQueueCreate(32, sizeof(char));
  if (xQueueBut == NULL)
    printf("falha em criar a queue \n");
  xSemaphoreBut1 = xSemaphoreCreateBinary();
  if (xSemaphoreBut1 == NULL)
    printf("falha em criar o semaforo \n");

  xSemaphoreBut3 = xSemaphoreCreateBinary();
  if (xSemaphoreBut3 == NULL)
    printf("falha em criar o semaforo \n");

  /* cria queue com 32 "espacos" */
  /* cada espa�o possui o tamanho de um inteiro*/
  xQueueLedFreq = xQueueCreate(32, sizeof(uint32_t));
  if (xQueueLedFreq == NULL)
    printf("falha em criar a queue \n");


  /* Create task to make led blink */
  if (xTaskCreate(task_led, "Led", TASK_LED_STACK_SIZE, NULL,
                  TASK_LED_STACK_PRIORITY, NULL) != pdPASS)
  {
    printf("Failed to create test led task\r\n");
  }

  /* Create task to monitor processor activity */
  if (xTaskCreate(task_but, "BUT", TASK_BUT_STACK_SIZE, NULL,
                  TASK_BUT_STACK_PRIORITY, NULL) != pdPASS)
  {
    printf("Failed to create UartTx task\r\n");
  }

  /* Start the scheduler. */
  vTaskStartScheduler();

  /* RTOS n�o deve chegar aqui !! */
  while (1)
  {
  }

  /* Will only get here if there was insufficient memory to create the idle
   * task. */
  return 0;
}
