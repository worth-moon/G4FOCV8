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
#include "key_app.h"
#include "key.h"

#include "debug_printf.h"
#include "vofa.h"

#include "string.h"

#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
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
#define RGB_Pin GPIO_PIN_5
#define RGB_GPIO_Port GPIOA
#define KEY4_Pin GPIO_PIN_6
#define KEY4_GPIO_Port GPIOC
#define KEY3_Pin GPIO_PIN_7
#define KEY3_GPIO_Port GPIOC
#define KEY2_Pin GPIO_PIN_8
#define KEY2_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_9
#define KEY1_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */
#define LED1_ON() HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1)
#define LED1_OFF() HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 0)

#define LED2_ON() HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1)
#define LED2_OFF() HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0)

#define LED3_ON() HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 1)
#define LED3_OFF() HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, 0)

#define RGB_ON() HAL_GPIO_WritePin(RGB_GPIO_Port, RGB_Pin, 1)
#define RGB_OFF() HAL_GPIO_WritePin(RGB_GPIO_Port, RGB_Pin, 0)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
