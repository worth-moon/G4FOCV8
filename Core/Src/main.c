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
#include "adc.h"
#include "spi.h"
#include "tim.h"
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

#include "trans.h"
#include "mt6816.h"
#include "foc.h"
#include "pid.h"
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
volatile float Ia_offset, Ib_offset, Ic_offset;

int foc_start_flag = 0;

volatile float mt6816_angle;

Pid_Controller_t GI_D;
Pid_Controller_t GI_Q;
Pid_Controller_t GVEL;
Pid_Controller_t GPOS;
//extern float theta_angle;
//extern float Vq;
//extern float Tcmp1, Tcmp2, Tcmp3;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void svpwm(int angle, float m);
void ADC_Count_Caloffset();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
* 测相电阻  0.0925  Ω
*   黑白 0.186    0.187/0.185
*   黑灰 0.185    0.185
*   白灰          0.185
* 测相电感
*   q               25.2545微亨
*     50.076
*     50.454
*     50.509
*   d               20.281
*       40.873
*       40.562
*       40.748
*   黑白 
* 
* 测好了，然后决定用现成的
* 
*/

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
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_USB_Device_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim3);
	
  Key_Init();
  Add_Key_demo();
	
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
 
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
	
	
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, __HAL_TIM_GET_AUTORELOAD(&htim1) - 10);
  HAL_ADCEx_InjectedStart_IT(&hadc1);
  ADC_Count_Caloffset();

  MT6816_SPI_Signal_Init();
  PID_param_init();

  Pid_Init(&GI_D, GI_D_KP, GI_D_KI, GI_D_KD, GI_D_KIS, 1.0f / GI_D_FREQUENCY, GI_D_RANGE);
  Pid_Init(&GI_Q, GI_Q_KP, GI_Q_KI, GI_Q_KD, GI_Q_KIS, 1.0f / GI_D_FREQUENCY, GI_Q_RANGE);
  Pid_Init(&GVEL, GVEL_KP, GVEL_KI, GVEL_KD, GVEL_KIS, 1.0f / GVEL_FREQUENCY, GVEL_RANGE);
  Pid_Init(&GPOS, GPOS_KP, GPOS_KI, GPOS_KD, GPOS_KIS, 1.0f / GPOS_FREQUENCY, GPOS_RANGE);
  foc_start_flag = 1;
	
  my_printf("setup done\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
				 

          vofa_demo();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
          //如果波形异常，可能是这俩语句占用时间
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 85;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
    int16_t a, b, c, bus;
    uint16_t Ia_raw, Ib_raw, Ic_raw;
void ADC_Count_Caloffset()
{

    for (int i = 0; i < 16; ++i)
    {
        HAL_Delay(1);
        Ia_raw = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
        Ib_raw = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
        Ic_raw = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);
        a += Ia_raw;
        b += Ib_raw;
        c += Ic_raw;
    }
    //右移四位，相当于 ÷16
    Ia_offset = a / 16.0f;
    Ib_offset = b / 16.0f;
    Ic_offset = c / 16.0f;

    my_printf("Ia_offset = %f, Ib_offset = %f, Ic_offset = %f\r\n", Ia_offset, Ib_offset, Ic_offset);
}

float ch1, ch2, ch3;						//通道1、2、3的装载值
void svpwm(int angle, float m)
{
	theta_angle = angle;
	Vq = m;
	
    tran_angle(theta_angle);                //角度转弧度
    sin_cos_val();                          //三角变换
    anti_park_transf();                     //旋转转静止坐标轴
    svpwm_calc();                           //SVPWM转三相
    
    TIM1->CCR1 = (uint16_t)(Tcmp1);
    TIM1->CCR2 = (uint16_t)(Tcmp2);
    TIM1->CCR3 = (uint16_t)(Tcmp3);
}


volatile float adc1_current, adc2_current, adc3_current,test_acc_angle = 0.5;
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == ADC1)
	{
        HAL_GPIO_WritePin(CH1_GPIO_Port, CH1_Pin, GPIO_PIN_SET);
        //电流采样
        adc1_current = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);
        adc2_current = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2);
        adc3_current = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3);
        //* 3.3/4096/0.1    0.008056640625
        //Ia = (float)(adc1_current - Ia_offset) * ADC_REF / ADC_12BIT / (CURRENT_RS * OPAMP_AU);
        //Ic = (float)(adc2_current - Ib_offset) * ADC_REF / ADC_12BIT / (CURRENT_RS * OPAMP_AU);
        //Ib = (float)(adc3_current - Ic_offset) * ADC_REF / ADC_12BIT / (CURRENT_RS * OPAMP_AU);

        //Ia = -Ia;
        //Ib = -Ib;
        //Ic = -Ic;

        Ia = -(float)(adc1_current - Ia_offset) * 0.008056640625f;
        Ic = -(float)(adc2_current - Ib_offset) * 0.008056640625f;
        Ib = -(float)(adc3_current - Ic_offset) * 0.008056640625f;

        //FOC_RUN
        if (foc_start_flag)
        {
						//HAL_GPIO_WritePin(CH2_GPIO_Port, CH2_Pin, GPIO_PIN_SET);
            Current_Closed_Loop();
             //观测电流
//             clark_transf();
//             park_transf();
                        //Position_Closed_Loop();
						//HAL_GPIO_WritePin(CH2_GPIO_Port, CH2_Pin, GPIO_PIN_RESET);
        }
        HAL_GPIO_WritePin(CH1_GPIO_Port, CH1_Pin, GPIO_PIN_RESET);

	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
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
