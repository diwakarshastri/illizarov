/**
  ******************************************************************************
  * @file    TIM/TIM_TimeBase/Src/main.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    16-September-2015
  * @brief   This sample code shows how to use STM32L4xx TIM HAL API to generate
  *          a time base of one second with the corresponding Interrupt request.
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
#include "flash.h"
#include "math.h"

/** @addtogroup STM32L4xx_HAL_Examples
  * @{
  */

/** @addtogroup TIM_TimeBase
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
static GPIO_InitTypeDef  GPIO_InitStruct;
TIM_Encoder_InitTypeDef Encoder;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
char isDemandChanged = 0, dir, control = PBC;
uint8_t iteration = 0;
uint32_t data_index2 = 0,data_index1=0, local_demand=0,previous_demand=0, demand = 0;
uint64_t emk[100][3]={0}, local_db[100][3]={0};
int32_t error=0;
double tOn1=0, tOn2=0, pOn1=0, pOn2=0, pS1=0, pS2=0;
float pcc=1;
uint32_t pOn=0, tOn=0, pS=0, pSa;
extern int32_t present_encoder_count,previous_encoder_count;
uint32_t local_time;

/* TIM handle declaration */
TIM_HandleTypeDef    TimHandle_int;
TIM_HandleTypeDef    TimHandle_Enc1;
TIM_HandleTypeDef    TimHandle_Enc2;
TIM_HandleTypeDef    TimHandle_Enc3;
TIM_HandleTypeDef    TimHandle_Enc4;

/* Prescaler declaration */
uint32_t uwPrescalerValue = 0;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
//static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 144 MHz */
  SystemClock_Config();
	
	/* Enable Peripheral clock */{
	__HAL_RCC_GPIOA_CLK_ENABLE() ;
	__HAL_RCC_GPIOB_CLK_ENABLE() ;
	__HAL_RCC_TIM2_CLK_ENABLE() ;
	__HAL_RCC_TIM3_CLK_ENABLE() ;
	__HAL_RCC_TIM4_CLK_ENABLE() ;
	__HAL_RCC_TIM5_CLK_ENABLE() ;
	__HAL_RCC_TIM7_CLK_ENABLE() ;}

			
	/* Configure Encoders 
	
	Encoder timers are configured at 100u second */{
	
				/*------------------------ Encoder 1 ------------------------------*/
				/* Encoder 1 Timer Configuration */{
				TimHandle_Enc1.Instance = TIM2;
	
				Encoder.EncoderMode = TIM_ENCODERMODE_TI12;
				Encoder.IC1Polarity = TIM_ICPOLARITY_RISING;
				Encoder.IC2Polarity = TIM_ICPOLARITY_RISING;
				Encoder.IC1Selection=TIM_ICSELECTION_DIRECTTI;
				Encoder.IC2Selection=TIM_ICSELECTION_DIRECTTI;
				Encoder.IC1Prescaler=TIM_ICPSC_DIV1;
				Encoder.IC2Prescaler=TIM_ICPSC_DIV1;
	
				TimHandle_Enc1.Init.Period            = 14400;
				TimHandle_Enc1.Init.Prescaler         = 0;
				TimHandle_Enc1.Init.CounterMode       = TIM_COUNTERMODE_UP;
	
				if (HAL_TIM_Encoder_Init(&TimHandle_Enc1, &Encoder) != HAL_OK)
				{
					/* Starting Error */
					while(1);
				}
				HAL_TIM_Encoder_Start(&TimHandle_Enc1,TIM_CHANNEL_ALL);}
				
				/* Encoder 1 GPIO Configuration */{

				GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
				GPIO_InitStruct.Pull  = GPIO_PULLUP;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
				GPIO_InitStruct.Alternate = GPIO_AF1_TIM2 ;

				GPIO_InitStruct.Pin = GPIO_PIN_15;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);				
					
				GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
				GPIO_InitStruct.Pull  = GPIO_PULLUP;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
				GPIO_InitStruct.Alternate = GPIO_AF1_TIM2 ;

				GPIO_InitStruct.Pin = GPIO_PIN_3;
				HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);}
				/*-----------------------------------------------------------------*/
				/*------------------------ Encoder 2 ------------------------------*/
				/* Encoder 2 Timer Configuration */{
				TimHandle_Enc2.Instance = TIM3;
	
				Encoder.EncoderMode = TIM_ENCODERMODE_TI12;
				Encoder.IC1Polarity = TIM_ICPOLARITY_RISING;
				Encoder.IC2Polarity = TIM_ICPOLARITY_RISING;
				Encoder.IC1Selection=TIM_ICSELECTION_DIRECTTI;
				Encoder.IC2Selection=TIM_ICSELECTION_DIRECTTI;
				Encoder.IC1Prescaler=TIM_ICPSC_DIV1;
				Encoder.IC2Prescaler=TIM_ICPSC_DIV1;
	
				TimHandle_Enc2.Init.Period            = 14400;
				TimHandle_Enc2.Init.Prescaler         = 0;
				TimHandle_Enc2.Init.CounterMode       = TIM_COUNTERMODE_UP;
	
				if (HAL_TIM_Encoder_Init(&TimHandle_Enc2, &Encoder) != HAL_OK)
				{
					/* Starting Error */
					while(1);
				}
				HAL_TIM_Encoder_Start(&TimHandle_Enc2,TIM_CHANNEL_ALL);}

				/* Encoder 2 GPIO Configuration */{

				GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
				GPIO_InitStruct.Pull  = GPIO_PULLUP;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
				GPIO_InitStruct.Alternate = GPIO_AF2_TIM3 ;

				GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	}
				/*-----------------------------------------------------------------*/
				/*------------------------ Encoder 3 ------------------------------*/
				/* Encoder 3 Timer Configuration */{
				TimHandle_Enc3.Instance = TIM4;
	
				Encoder.EncoderMode = TIM_ENCODERMODE_TI12;
				Encoder.IC1Polarity = TIM_ICPOLARITY_RISING;
				Encoder.IC2Polarity = TIM_ICPOLARITY_RISING;
				Encoder.IC1Selection=TIM_ICSELECTION_DIRECTTI;
				Encoder.IC2Selection=TIM_ICSELECTION_DIRECTTI;
				Encoder.IC1Prescaler=TIM_ICPSC_DIV1;
				Encoder.IC2Prescaler=TIM_ICPSC_DIV1;
	
				TimHandle_Enc3.Init.Period            = 14400;
				TimHandle_Enc3.Init.Prescaler         = 0;
				TimHandle_Enc3.Init.CounterMode       = TIM_COUNTERMODE_UP;
	
				if (HAL_TIM_Encoder_Init(&TimHandle_Enc3, &Encoder) != HAL_OK)
				{
					/* Starting Error */
					while(1);
				}
				HAL_TIM_Encoder_Start(&TimHandle_Enc3,TIM_CHANNEL_ALL);}

				/* Encoder 3 GPIO Configuration */{

				GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
				GPIO_InitStruct.Pull  = GPIO_PULLUP;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
				GPIO_InitStruct.Alternate = GPIO_AF2_TIM4 ;

				GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
				HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);}	
				/*-----------------------------------------------------------------*/
				/*------------------------ Encoder 4 ------------------------------*/
				/* Encoder 4 Timer Configuration */{
				TimHandle_Enc4.Instance = TIM5;
	
				Encoder.EncoderMode = TIM_ENCODERMODE_TI12;
				Encoder.IC1Polarity = TIM_ICPOLARITY_RISING;
				Encoder.IC2Polarity = TIM_ICPOLARITY_RISING;
				Encoder.IC1Selection=TIM_ICSELECTION_DIRECTTI;
				Encoder.IC2Selection=TIM_ICSELECTION_DIRECTTI;
				Encoder.IC1Prescaler=TIM_ICPSC_DIV1;
				Encoder.IC2Prescaler=TIM_ICPSC_DIV1;
	
				TimHandle_Enc4.Init.Period            = 14400;
				TimHandle_Enc4.Init.Prescaler         = 0;
				TimHandle_Enc4.Init.CounterMode       = TIM_COUNTERMODE_UP;
	
				if (HAL_TIM_Encoder_Init(&TimHandle_Enc4, &Encoder) != HAL_OK)
				{
					/* Starting Error */
					while(1);
				}
				HAL_TIM_Encoder_Start(&TimHandle_Enc4,TIM_CHANNEL_ALL);}

				/* Encoder 4 GPIO Configuration */{

				GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
				GPIO_InitStruct.Pull  = GPIO_PULLUP;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
				GPIO_InitStruct.Alternate = GPIO_AF2_TIM5 ;

				GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);}}	
				/*-----------------------------------------------------------------*/

	/* Configure Timer interrrupt */{
		
				/* Timer 7 interrupt is configured at 1m second */{
		
				TimHandle_int.Instance = TIM7;

				TimHandle_int.Init.Period      = 14400;
				TimHandle_int.Init.Prescaler   = 9;
				TimHandle_int.Init.CounterMode = TIM_COUNTERMODE_UP;

				if (HAL_TIM_Base_Init(&TimHandle_int) != HAL_OK)
				{
					/* Initialization Error */
				}}	
			
				/* Set interrupt priority*/{	
				HAL_NVIC_SetPriority(TIM7_IRQn, 3, 0);}

				/* Enable the TIMx global Interrupt */{
				HAL_NVIC_EnableIRQ(TIM7_IRQn);}}

	/* GPIO Configuration for motor control */{
				/* Motor 1 GPIO Configuration */{

				GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
				GPIO_InitStruct.Pull  = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

				GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_10;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);}
	
	/* Reset motors */
	
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 , GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);}
			
	
	
	/* Read EMK and copy to local database */{
	
				flash_read(300);
				for(data_index1=0;data_index1<100;data_index1++)
					for(data_index2=0;data_index2<3;data_index2++)
						local_db[data_index1][data_index2]=emk[data_index1][data_index2];}
	
	/* Reset Encoders */{
		
				__HAL_TIM_SET_COUNTER(&TimHandle_Enc1,0);
				__HAL_TIM_SET_COUNTER(&TimHandle_Enc2,0);
				__HAL_TIM_SET_COUNTER(&TimHandle_Enc3,0);
				__HAL_TIM_SET_COUNTER(&TimHandle_Enc4,0);}

	while (1)
  {
		if(isDemandChanged)
		{

				/* Search the data base and get previous and immediate next data points */
				for(data_index1=0;data_index1<100;data_index1++)
					if(local_db[data_index1][2] > local_demand )
						break;
					
		/*
				 /\		|	
				/  \		|
			 	 ||		|
				 ||		|
					position|
						|
				 	pS	|_________________________________________._._._._.
						|  				       '  											 '
						|				     '   												'
						|				   ' 						 						'
						|				 '													 '
						|				'	 													'---------> dynamic positon of the motor 
						|         	 	       '							 	'
						|			     '  									 '
						|			    '   									 '
						|		   	   '  								'
						|_______________________ -'-----> pOn 
						|	            	|
						|		      ' |
						|             	    ' 	|
						|             '    	|-----> pulse to turn on the motor 	
						| 	'       	|
						|     '           	|
						|    '              	|
						|'                  	|
						|_______________________|_____________________________
						|<---------tOn--------->|											 t->
					
					pOn -> motor position at the moment when pulse is turned off
					tOn -> duration for which motor is turned on 
					pS  -> motor settling position
		*/
		
				/* Get previous values for interpolation */
				tOn1 = (double)(local_db[data_index1-1][0]) ; 	// Previous tOn
				tOn2 = (double)(local_db[data_index1][0])   ;	 	// Next tOn
				pOn1 = (double)(local_db[data_index1-1][1]) ;		// Previous pOn
				pOn2 = (double)(local_db[data_index1][1])   ;		// Next pOn	
				pS1  = (double)(local_db[data_index1-1][2]) ;		// Previous pS
				pS2  = (double)(local_db[data_index1][2])   ;		// Next pS
				
				/* Interpolation */
				pOn  = (uint32_t)(pOn1 + (pOn2 - pOn1)*((((double)(local_demand)) - pS1)/(pS2 - pS1))); // Predict pOn
				tOn  = (uint32_t)(tOn1 + (tOn2 - tOn1)*((((double)(local_demand)) - pS1)/(pS2 - pS1)));	// Predict tOn
				
				/* Adapptation */
				pOn = (uint32_t)(((float)pOn)*pcc);
				
				/* Define pOn depending on direction */
				if(dir == pos)
					pOn = pSa + pOn;
				else
					pOn = pSa - pOn;
				
				/* If Time based control, the pulse will be on untill local_time equals tOn */
				local_time = 0;
				
				/* Depending on the direction, turn on the motor */				
				if(dir == pos)
				{
					/* Turn on the motor in CW direction */
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 , GPIO_PIN_SET  );
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);

				}
				else if(dir == rev)
				{
					/* Turn on the motor in Anti-CW direction */
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8 , GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET  );
				}

				/* Start timer interrupt */				
				HAL_TIM_Base_Start(&TimHandle_int);
				HAL_TIM_Base_Start_IT(&TimHandle_int);
								
				iteration++;
				isDemandChanged = 0;
		}
		
		/* Update new demand */
		if(previous_demand != demand )
		{
			
				pSa = __HAL_TIM_GET_COUNTER(&TimHandle_Enc1);
				local_demand = absolute(demand - pSa);
				error=0;
			
				/* Set direction*/
				if(demand < pSa)
					dir = rev;
				else
					dir = pos;

				/* Decide control algorithm based on demand */
				if(local_demand <= 100)
					control = TBC;	// Time Based Control (TBC = 0)
				else
					control = PBC;	// Position Based Control (PBC = 1)
				
				iteration = 0;
				previous_demand = demand;
				isDemandChanged = 1;
		}	

  }

}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
//  BSP_LED_Toggle(LED2);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 144 MHz
  *            HCLK(Hz)                       = 144 MHz
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 48 MHz
  *            PLL_M                          = 1
  *            PLL_N                          = 6
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            PLL_Q                          = 4
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* MSI is enabled after System reset, activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 6;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
//static void Error_Handler(void)
//{
//  while (1)
//  {
//  }
//}
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
