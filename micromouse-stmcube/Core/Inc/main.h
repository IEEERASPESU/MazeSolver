/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32h5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY_K2_Pin GPIO_PIN_13
#define KEY_K2_GPIO_Port GPIOC
#define DIP0_Pin GPIO_PIN_15
#define DIP0_GPIO_Port GPIOC
#define XSHUT_0_Pin GPIO_PIN_0
#define XSHUT_0_GPIO_Port GPIOC
#define DIP1_Pin GPIO_PIN_1
#define DIP1_GPIO_Port GPIOC
#define XSHUT_1_Pin GPIO_PIN_2
#define XSHUT_1_GPIO_Port GPIOC
#define DIP2_Pin GPIO_PIN_3
#define DIP2_GPIO_Port GPIOC
#define XSHUT_2_Pin GPIO_PIN_0
#define XSHUT_2_GPIO_Port GPIOA
#define DIP3_Pin GPIO_PIN_1
#define DIP3_GPIO_Port GPIOA
#define XSHUT_3_Pin GPIO_PIN_2
#define XSHUT_3_GPIO_Port GPIOA
#define DIP4_Pin GPIO_PIN_3
#define DIP4_GPIO_Port GPIOA
#define XSHUT_4_Pin GPIO_PIN_4
#define XSHUT_4_GPIO_Port GPIOA
#define DIP5_Pin GPIO_PIN_5
#define DIP5_GPIO_Port GPIOA
#define DIP6_Pin GPIO_PIN_7
#define DIP6_GPIO_Port GPIOA
#define PWM2_Pin GPIO_PIN_4
#define PWM2_GPIO_Port GPIOC
#define DIP7_Pin GPIO_PIN_5
#define DIP7_GPIO_Port GPIOC
#define LED_D1_Pin GPIO_PIN_2
#define LED_D1_GPIO_Port GPIOB
#define BIN2_Pin GPIO_PIN_6
#define BIN2_GPIO_Port GPIOC
#define BIN1_Pin GPIO_PIN_7
#define BIN1_GPIO_Port GPIOC
#define MH1_MH2_Pin GPIO_PIN_8
#define MH1_MH2_GPIO_Port GPIOA
#define PWM1_Pin GPIO_PIN_15
#define PWM1_GPIO_Port GPIOA
#define AIN1_Pin GPIO_PIN_5
#define AIN1_GPIO_Port GPIOB
#define AIN2_Pin GPIO_PIN_6
#define AIN2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
