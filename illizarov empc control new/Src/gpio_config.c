#include "main.h"
static GPIO_InitTypeDef  GPIO_InitStruct;

void gpio_config(GPIO_TypeDef  *GPIOxx, uint16_t pin, uint32_t mode, uint32_t pull, uint32_t speed, char gpio_fn, uint8_t Alternate_Fn)
{
	GPIO_InitStruct.Mode  = mode;
	GPIO_InitStruct.Pull  = pull;
	GPIO_InitStruct.Speed = speed;
	if(gpio_fn)
	GPIO_InitStruct.Alternate = AF ;

	GPIO_InitStruct.Pin = pin;
	HAL_GPIO_Init(GPIOxx, &GPIO_InitStruct);
}
