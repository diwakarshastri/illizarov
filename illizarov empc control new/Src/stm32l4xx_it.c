/**
  ******************************************************************************
  * @file    TIM/TIM_TimeBase/Src/stm32l4xx_it.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    16-September-2015
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_it.h"
#include "math.h"

/** @addtogroup STM32L4xx_HAL_Examples
  * @{
  */

/** @addtogroup TIM_TimeBase
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
extern TIM_HandleTypeDef    TimHandle_int;
extern TIM_HandleTypeDef    TimHandle_Enc1;
extern TIM_HandleTypeDef    TimHandle_Enc2;
extern TIM_HandleTypeDef    TimHandle_Enc3;
extern TIM_HandleTypeDef    TimHandle_Enc4;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern double tOn1, tOn2, pOn1, pOn2, pS1, pS2;
extern float pcc;
char isHigh =0;
extern char isDemandChanged, dir, control;
extern uint32_t tOn, pOn, pS, demand, pSa;
extern int32_t error;
extern uint32_t local_demand;
extern uint32_t local_time;
int32_t present_encoder_count=0,previous_encoder_count=0,offset=0;
int32_t speed=0,speed_count=0,local_time_ms=0,lut_index=0,x=1,y=4;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
uint32_t absolute(int32_t value)
{
	if (value < 0) {
    return -value;
  }
  else {
    return value;  
  }
}


/******************************************************************************/
/*                 STM32L4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l4xx.s).                                               */
/******************************************************************************/



/**
  * @Encoder 
  */

void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle_Enc1);
}
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle_Enc2);
}
void TIM4_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle_Enc3);
}
void TIM5_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&TimHandle_Enc4);
}

/**
  * @brief  This function handles TIM interrupt request.
  * @param  None
  * @retval None
  */
void TIM7_IRQHandler(void)
{
			present_encoder_count = __HAL_TIM_GET_COUNTER(&TimHandle_Enc1);
			speed = present_encoder_count - previous_encoder_count ;
			local_time++;
	
			/* If control is PBC, turn off the motor when the present_encoder_count is greater than or equal to pOn */
			if(control == PBC && ((dir == pos && present_encoder_count >= pOn) || (dir == rev && present_encoder_count <= pOn)))
			{
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 , GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
				isHigh = 1;			// Calculate PCC for adaptation
			}
			
			/* If control is TBC, turn off the motor when the local_time is greater than or equal to tOn */
			if(control == TBC && local_time>=tOn)
			{	
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 , GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);	
			}
	
			/* Wait until motor settles */
			if(speed_count == 10 ) 
			{
				pS = present_encoder_count ;
								
				if(isHigh)		// If the control was PBC, calculate PCC for adaptation
				{
					pcc = (((float)x*pcc)+((float)y*pcc*((float)local_demand/(float)(absolute(pS - pSa)))))/((float)x+(float)y);
					isHigh = 0;}
				
				error = demand - pS;
				local_demand = absolute(error);
				pSa = __HAL_TIM_GET_COUNTER(&TimHandle_Enc1);

				if(local_demand <= 6);						// If error is less than 6 pulses, exit the control loop			
				else if(local_demand <= 100){			// If error is less than 100 pulses, use Time Based Control
					control = TBC;
					local_time=0;
					isDemandChanged=1;}
				else { 														// If error is greater tha 100 pulses, use Position Based Control
					control = PBC;
					isDemandChanged=1;}
				
				/* Set direction depending on the sign of the error */
				if(error < 0)
					dir = rev;
				else
					dir = pos;
				
				/* Disable Timer Interrupt */
				HAL_TIM_Base_Stop(&TimHandle_int);
				
				/* Reset Speed count*/
				speed_count = 0;
			}
			
			/* Wait for 10ms after speed becomes 0 ie., until the motor settles */
			if(speed == 0)
				speed_count++;		// If speed is 0, increment speed count
			else
				speed_count = 0;	// Else reset speed count 

			previous_encoder_count = present_encoder_count;
			HAL_TIM_IRQHandler(&TimHandle_int);
}

/**
  * @}
  */

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
