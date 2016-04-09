#include "main.h"
TIM_Encoder_InitTypeDef Encoder;

void encoder_config(TIM_HandleTypeDef *htim, TIM_TypeDef *TIM , uint32_t period , uint32_t prescalar)
{
	
	htim->Instance = TIM;
	
	Encoder.EncoderMode = TIM_ENCODERMODE_TI12;
	Encoder.IC1Polarity = TIM_ICPOLARITY_RISING;
	Encoder.IC2Polarity = TIM_ICPOLARITY_RISING;
	Encoder.IC1Selection=TIM_ICSELECTION_DIRECTTI;
	Encoder.IC2Selection=TIM_ICSELECTION_DIRECTTI;
	Encoder.IC1Prescaler=TIM_ICPSC_DIV1;
	Encoder.IC2Prescaler=TIM_ICPSC_DIV1;
	
  htim->Init.Period            = period;
  htim->Init.Prescaler         = prescalar;
  htim->Init.CounterMode       = TIM_COUNTERMODE_UP;
	
	  if (HAL_TIM_Encoder_Init(htim, &Encoder) != HAL_OK)
  {
    /* Starting Error */

  }
	HAL_TIM_Encoder_Start(htim,TIM_CHANNEL_ALL);

}


void interrupt_config(TIM_HandleTypeDef *htim, TIM_TypeDef *TIM , uint32_t period , uint32_t prescalar)
{
  htim->Instance = TIM;

  htim->Init.Period            = period;
  htim->Init.Prescaler         = prescalar;
  htim->Init.CounterMode       = TIM_COUNTERMODE_UP;

  if (HAL_TIM_Base_Init(htim) != HAL_OK)
  {
    /* Initialization Error */
//    Error_Handler();
  }
//if (HAL_TIM_Base_Start(htim) != HAL_OK)
//  {
//    /* Initialization Error */
////    Error_Handler();
//  }
//  /*##-2- Start the TIM Base generation in interrupt mode ####################*/
//  /* Start Channel1 */
//  if (HAL_TIM_Base_Start_IT(htim) != HAL_OK)
//  {
//    /* Starting Error */
////    Error_Handler();
//  }

}
