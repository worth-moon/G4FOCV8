/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

#include "key_app.h"
#include "key.h"

#include "debug_printf.h"
#include "vofa.h"

#include "string.h"
#include "math.h"

#include "trans.h"
#include "mt6816.h"

#include "position_pid.h"
#include "pid.h"
#include "foc.h"
/* USER CODE END Includes */

/* Exported types -----1------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum {
    WAITING,              // 等待（空闲状态）
    BUTTON1_SHORT_PRESS,  // 按键1短按
    BUTTON1_LONG_PRESS,   // 按键1长按
    BUTTON2_SHORT_PRESS,  // 按键2短按
    BUTTON2_LONG_PRESS,   // 按键2长按
    BUTTON3_SHORT_PRESS,  // 按键3短按
    BUTTON3_LONG_PRESS,   // 按键3长按
    BUTTON4_SHORT_PRESS,  // 按键4短按
    BUTTON4_LONG_PRESS    // 按键4长按
} ButtonState;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
extern ButtonState button;
extern const char* buttonStateStrings[];
extern float ch1, ch2, ch3;
extern Pid_Controller_t GI_D,GI_Q,GVEL,GPOS;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_14
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_15
#define LED3_GPIO_Port GPIOC
#define APH4_Pin GPIO_PIN_3
#define APH4_GPIO_Port GPIOC
#define RGB_Pin GPIO_PIN_5
#define RGB_GPIO_Port GPIOA
#define CH1_Pin GPIO_PIN_10
#define CH1_GPIO_Port GPIOB
#define CH2_Pin GPIO_PIN_11
#define CH2_GPIO_Port GPIOB
#define LIN1_Pin GPIO_PIN_13
#define LIN1_GPIO_Port GPIOB
#define LIN2_Pin GPIO_PIN_14
#define LIN2_GPIO_Port GPIOB
#define LIN3_Pin GPIO_PIN_15
#define LIN3_GPIO_Port GPIOB
#define KEY4_Pin GPIO_PIN_6
#define KEY4_GPIO_Port GPIOC
#define KEY3_Pin GPIO_PIN_7
#define KEY3_GPIO_Port GPIOC
#define KEY2_Pin GPIO_PIN_8
#define KEY2_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_9
#define KEY1_GPIO_Port GPIOC
#define HIN1_Pin GPIO_PIN_8
#define HIN1_GPIO_Port GPIOA
#define HIN2_Pin GPIO_PIN_9
#define HIN2_GPIO_Port GPIOA
#define HIN3_Pin GPIO_PIN_10
#define HIN3_GPIO_Port GPIOA
#define CSN_Pin GPIO_PIN_2
#define CSN_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */
#define LED1_ON() HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1)
#define LED1_OFF() HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0)

#define LED2_ON() HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1)
#define LED2_OFF() HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0)

#define LED3_ON() HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1)
#define LED3_OFF() HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0)

#define RGB_ON() HAL_GPIO_WritePin(RGB_GPIO_Port, RGB_Pin, 1)
#define RGB_OFF() HAL_GPIO_WritePin(RGB_GPIO_Port, RGB_Pin, 0)


#define ADC_12BIT (4096.0f)
#define ADC_REF (3.3f)
#define OPAMP_AU (20.0f)
#define CURRENT_RS (0.005f)

#define GI_D_FREQUENCY (10000.0f)
#define GI_D_KP (0.011175f)//1.0f Drag_IF_Mode
#define GI_D_KI (56.23704f)//24.0f  Drag_IF_Mode
#define GI_D_KD (0.0f)
#define GI_D_KIS (20.0f)
#define GI_D_RANGE (1.0f)

#define GI_Q_FREQUENCY (10000.0f)
#define GI_Q_KP (0.012835f)//1.0f Drag_IF_Mode
#define GI_Q_KI (56.23704f)//24.0f  Drag_IF_Mode
#define GI_Q_KD (0.0f)
#define GI_Q_KIS (20.0f)
#define GI_Q_RANGE (5.0f)

#define GVEL_FREQUENCY (5000.0f)
#define GVEL_KP (0.01f)
#define GVEL_KI (0.03f)
#define GVEL_KD (0.0f)
#define GVEL_KIS (0.0f)
#define GVEL_RANGE (3.0f)

#define GPOS_FREQUENCY (2500.0f)
#define GPOS_KP (20.0f)
#define GPOS_KI (0.00f)
#define GPOS_KD (10.0f)
#define GPOS_KIS (0.0f)
#define GPOS_RANGE (300.0f)//根据vq，也就是电流环的范围调，别超过最大速度的才好

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
