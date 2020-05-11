/**
  ******************************************************************************
  * File Name          : TIM.c
  * Description        : This file provides code for the configuration
  *                      of the TIM instances.
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

/* Includes ------------------------------------------------------------------*/
#include "tim.h"
#include "fifo.h"

/* USER CODE BEGIN 0 */
TIM_HandleTypeDef htim3;
extern fifo_typedef SRF_fifo;
/* USER CODE END 0 */

//TIM_HandleTypeDef htim3;

/* TIM3 init function */

void MX_TIMER_Init(void)
{
  //Enabler Timer clock
  __HAL_RCC_TIM3_CLK_ENABLE();
//  mw32(0x40021038,0x10020001); // Enable TIM3 clock
  //Configure Timer base
  htim3.Instance = TIM3;
  htim3.Instance->PSC = 0;
  htim3.Instance->ARR = 0xFFFF;
//  htim3.Instance->CR1 = TIM_CR1_CEN;

//  MX_TIMERIC_Init();
  MX_TIMERIC_Init();

 /* Configure interrupt */ 
  HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);

/* Timer needs to be enabled by writing 0x01 to CR1 */
  MX_TIMER_Enable();
}

void MX_TIMERIC_Init(void)
{
  //Disable Timer before changing the IC registers
  htim3.Instance->CR1 = 0x00;
  //Set timer base
  htim3.Instance->PSC = 0;
  htim3.Instance->ARR = 0xFFFF;

  //Configure input capture IC1 and IC2, PB4 --> IC1/2
  htim3.Instance->SR = 0x0000; // Clean interrupt status register
  htim3.Instance->CCMR1 = 0x0201; // IC2 = TI1, IC1 = TI1
  htim3.Instance->CCMR2 = 0x0201; // IC3 = TI1, IC4 = TI1
  htim3.Instance->CCER = 0x3131; // IC2 falling edge, IC1 rising edge; IC4 falling, IC3 rising
  htim3.Instance->DIER = 0x10; // IC4 interrupt enabled (falling edge)
//  htim3.Instance->CR1 = TIM_CR1_CEN;
}

void MX_TIMER_Enable(void)
{
	htim3.Instance->CR1 = 0x01;
}

void MX_TIMER_Disable(void)
{
	htim3.Instance->CR1 = 0x00;
}

/* USER CODE BEGIN 1 */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM30 */
  // As the processing time is much faster in comparison to the uart baudrate
  // The UART interrupt request is usually completed before the next interrupt

  uint32_t volatile isrflags   = READ_REG(htim3.Instance->SR);
  uint16_t data_rising = 0; // CC register is 16bits
  uint16_t data_falling = 0; // CC register is 16bits
  uint16_t data = 0;

  if((isrflags & 0x10) != 0U) // CC2IF
  {
    data_rising = (htim3.Instance->CCR3) & 0xffff; //Rising edge capture counter
    data_falling =(htim3.Instance->CCR4) & 0xffff; //Falling edge capture counter
    if (data_rising<data_falling) data = data_falling-data_rising; 
    else data = 0xffff-data_rising+data_falling;

    fifoWrite(&SRF_fifo,(uint8_t) ((data>>8)&0xFF)); //Store higher 8bits
    fifoWrite(&SRF_fifo,(uint8_t) (data&0xFF)); //Store lower 8bits
  }
  htim3.Instance->SR=0;
  /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
