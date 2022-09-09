/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for upper_queue */
osMessageQueueId_t upper_queueHandle;
const osMessageQueueAttr_t upper_queue_attributes = {
  .name = "upper_queue"
};
/* Definitions for uart_tx_lock */
osSemaphoreId_t uart_tx_lockHandle;
const osSemaphoreAttr_t uart_tx_lock_attributes = {
  .name = "uart_tx_lock"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of uart_tx_lock */
  uart_tx_lockHandle = osSemaphoreNew(1, 1, &uart_tx_lock_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of upper_queue */
  upper_queueHandle = osMessageQueueNew (16, sizeof(uart_buf_t), &upper_queue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
    static uart_buf_t uart_rec;
    static uint8_t state = 0;
    static uint8_t buffer[512] = "";
    static uint16_t buffer_len = 0;
    static uint8_t send_buf[512];
  for(;;)
  {
    if (osOK ==
    	osMessageQueueGet(upper_queueHandle, &uart_rec, NULL, osWaitForever)) {
    	for (uint8_t i = 0; i < uart_rec.len; i++) {
    		switch (state) {
    		case 0:
    			if (uart_rec.buf[i] == '{') {
    				state ++;
    			}
    			break;
    		case 1:
    			if (uart_rec.buf[i] == '}') {

    				osSemaphoreAcquire(uart_tx_lockHandle, osWaitForever);
    				// make sure the buffer offered to dma do not change when transmitting.
    				memcpy(send_buf, buffer, buffer_len);
    				HAL_UART_Transmit_DMA(&huart2, send_buf, buffer_len);
//    				HAL_UART_Transmit(&huart2, buffer, buffer_len, 0xff);

    				state = 0;
    				memset(buffer, 0, sizeof(buffer));
    				buffer_len = 0;
    			} else {
    				buffer[buffer_len] = uart_rec.buf[i];
    				buffer_len ++;
    				if (buffer_len >= 512) {
    					buffer_len = 0;
    					memset(buffer, 0, sizeof(buffer));
    				}
    			}
    		}
    	}
    }
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

