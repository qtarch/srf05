/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define UART_TX GPIO_PIN_2 // PA2, USART2, TX
#define UART_RX GPIO_PIN_3 // PA3, USART2, RX
#define SRF_ECHO1 GPIO_PIN_3 // PB3, TIM3_CH2, IC2
#define SRF_TRIG GPIO_PIN_5 // PB5, GPIO, PP
#define SRF_ECHO GPIO_PIN_4 // PB4, TIM3_CH1, IC1
#define SRF_OUT GPIO_PIN_10 // PB10, NC
#define SRF_GND GPIO_PIN_8 // PA8, GPIO, PP
#define LD2 GPIO_PIN_5 // LED2, PA5, OD
#define BTN GPIO_PIN_14 // User Button, PC14, PP

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
