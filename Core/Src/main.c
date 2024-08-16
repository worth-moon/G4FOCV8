/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "key_app.h"
#include "key.h"

#include "debug_printf.h"
#include "vofa.h"

#include "string.h"
#include "math.h"
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
ButtonState button = WAITING;
const char* buttonStateStrings[] = {
    "WAITING",              // 等待（空闲状态）
    "BUTTON1_SHORT_PRESS",  // 按键1短按
    "BUTTON1_LONG_PRESS",   // 按键1长按
    "BUTTON2_SHORT_PRESS",  // 按键2短按
    "BUTTON2_LONG_PRESS",   // 按键2长按
    "BUTTON3_SHORT_PRESS",  // 按键3短按
    "BUTTON3_LONG_PRESS",   // 按键3长按
    "BUTTON4_SHORT_PRESS",  // 按键4短按
    "BUTTON4_LONG_PRESS"    // 按键4长按
};

float foc_angle;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void svpwm(int angle, float m);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  //MX_USB_Device_Init();
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_USART3_UART_Init();
  MX_USB_Device_Init();
  MX_TIM1_Init();
	//MX_USB_Device_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim3);
	
	
  Key_Init();
  Add_Key_demo();
	
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
  //
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);

  my_printf("setup done\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      foc_angle += 1;
      if (foc_angle > 360)
          foc_angle = 0;

      svpwm((int)foc_angle, 0.5);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      handleButtonState();
      task_run_key();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 12;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void svpwm(int angle, float m)
{
    uint16_t t4, t6, t0;
    uint8_t section;							//扇区
    uint16_t Ts = __HAL_TIM_GET_AUTORELOAD(&htim1);	//计时器周期
    float ch1, ch2, ch3;						//通道1、2、3的装载值
    section = angle / 60 + 1;					//得到角度对应的扇区
    angle %= 60;								//因为前面的算法只计算了0到60度

    /*得到矢量的作用时间，除以57.2958是把角度换算成弧度*/
    t4 = sinf((60 - angle) / 57.2958f) * Ts * m;
    t6 = sinf(angle / 57.2958f) * Ts * m;
    t0 = (Ts - t4 - t6) / 2;

    /*判断扇区，用7段式svpwm调制，得到三个通道的装载值*/
    switch (section)
    {
    case 1:
    {
        ch1 = t4 + t6 + t0;
        ch2 = t6 + t0;
        ch3 = t0;
    }break;

    case 2:
    {
        ch1 = t4 + t0;
        ch2 = t4 + t6 + t0;
        ch3 = t0;
    }break;

    case 3:
    {
        ch1 = t0;
        ch2 = t4 + t6 + t0;
        ch3 = t6 + t0;
    }break;

    case 4:
    {
        ch1 = t0;
        ch2 = t4 + t0;
        ch3 = t4 + t6 + t0;
    }break;

    case 5:
    {
        ch1 = t6 + t0;
        ch2 = t0;
        ch3 = t4 + t6 + t0;
    }break;

    case 6:
    {
        ch1 = t4 + t6 + t0;
        ch2 = t0;
        ch3 = t4 + t0;
    }break;

    default:
        break;
    }

    
    /*如果有需要可以让硬件输出波形*/
//    TIM1->CCR1 = ch1;
//    TIM1->CCR2 = ch2;
//    TIM1->CCR3 = ch3;

    vofa_send_data(1, ch1);
    vofa_send_data(2, ch2);
    vofa_send_data(3, ch3);
    vofa_send_data(4, foc_angle);
    vofa_sendframetail();
    //SendWave(ch1, ch2, ch3);					//使用串口示波器显示波形
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    //if (htim == &htim11)//f = 100 t = 10ms 
    //{

    //}
    //else if (htim == &htim12)//f = 10k t = 100us
    //{

    //}
    //else if (htim == &htim13)//f = 5k t = 200us
    //{

    //}
    //else if (htim == &htim14)//f = 1k t = 1ms 
    //{
    //    //按键检测提供1ms心跳
    //    
    //}

    if (htim == &htim3)
    {
        task_listen_key();
    }
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  my_printf("Error_Handler\r\n");
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
