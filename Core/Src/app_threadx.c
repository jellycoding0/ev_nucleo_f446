/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
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
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
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
/* USER CODE BEGIN PV */
TX_THREAD led_thread;
extern volatile uint8_t os_started; // main.c의 방어 잠금 플래그 참조
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void led_thread_entry(ULONG thread_input);
/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN App_ThreadX_Init */
  CHAR *pointer;
  (void)byte_pool;

  // 1. 바이트 풀로부터 스레드 전용 스택 메모리(1024바이트) 할당
  ret = tx_byte_allocate(byte_pool, (VOID **)&pointer, 1024, TX_NO_WAIT);
  if (ret != TX_SUCCESS)
  {
    return ret;
  }

  // 2. LED 스레드 생성 (우선순위 10, 자동 시작)
  ret = tx_thread_create(&led_thread, "LED Thread", led_thread_entry, 0,
                         pointer, 1024, 10, 10, TX_NO_TIME_SLICE, TX_AUTO_START);
  if (ret != TX_SUCCESS)
  {
    return ret;
  }

  // 3. 🚨 스레드 생성이 완벽히 성공하면, 비로소 TIM7 인터럽트 틱 서비스 루틴을 잠금 해제합니다!
  os_started = 1;
  /* USER CODE END App_ThreadX_Init */

  return ret;
}

/**
  * @brief  MX_ThreadX_Init
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */
void led_thread_entry(ULONG thread_input)
{
  // 1틱 = 10ms (100Hz 틱 기준)
  // 500ms 딜레이를 제공하기 위해 50틱 설정
  const ULONG sleep_ticks = 50;

  while (1)
  {
    // PA5 초록 LED 토글
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    // OS 커널 슬립 상태로 진입 (CPU 점유율 반납)
    tx_thread_sleep(sleep_ticks);
  }
}
/* USER CODE END 1 */
