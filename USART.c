
#include "stm32l1xx.h"

#include "stm32l1xx_ll_system.h"
#include "stm32l1xx_ll_gpio.h"
#include "stm32l1xx_ll_pwr.h"
#include "stm32l1xx_ll_rcc.h"
#include "stm32l1xx_ll_utils.h"
#include "stm32l1xx_ll_bus.h"
#include "stm32l1xx_ll_lcd.h"
#include "stm32l152_glass_lcd.h"

#include "stm32l1xx_ll_usart.h"
#include "String.h"

void SystemClock_Config(void);
void GPIO_USART_Configure(void);
void USART_Configure(void);
void USART_SendString(uint8_t*, uint8_t);
uint8_t recv_beffer[10];
uint8_t idx = 0;

int main()
{
	USART_Configure();
	
	LL_GPIO_InitTypeDef GPIO_InitStruct;
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	while(1)
	{
		while(LL_USART_IsActiveFlag_RXNE(USART2) == RESET);
		recv_beffer[idx++] = LL_USART_ReceiveData8(USART2);
		idx = idx % 10;
		
		if(strstr((const char*)recv_beffer,"\r\n") != NULL)
		{
			if(strstr((const char*)recv_beffer, "on") != NULL)
			{
				LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);
				LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_7);
				memset(recv_beffer, 0, 10);
				idx = 0;
			}
			else if(strstr((const char*)recv_beffer, "off") != NULL)
			{
				LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
				LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);
				memset(recv_beffer, 0, 10);
				idx = 0;
			}
		}
	}
}

void GPIO_USART_Configure(void)
{
	LL_GPIO_InitTypeDef gpio_conf;
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	
	gpio_conf.Pin = LL_GPIO_PIN_2;
	gpio_conf.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_conf.Pull = LL_GPIO_PULL_UP;
	gpio_conf.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_conf.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio_conf.Alternate = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOA,&gpio_conf);
	
	gpio_conf.Pin = LL_GPIO_PIN_3;
	LL_GPIO_Init(GPIOA,&gpio_conf);
}

void USART_Configure(void)
{
	LL_USART_InitTypeDef usart_conf;
	GPIO_USART_Configure();
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
	
	usart_conf.BaudRate = 9600;
	usart_conf.DataWidth = LL_USART_DATAWIDTH_8B;
	usart_conf.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	usart_conf.Parity = LL_USART_PARITY_NONE;
	usart_conf.StopBits = LL_USART_STOPBITS_1;
	usart_conf.OverSampling = LL_USART_OVERSAMPLING_16;
	usart_conf.TransferDirection = LL_USART_DIRECTION_TX_RX;
	
	LL_USART_Init(USART2, &usart_conf);
	LL_USART_Enable(USART2);
}

void USART_SendString(uint8_t* str, uint8_t size)
{
	uint8_t i = 0;
	while(i < size)
	{
			while(LL_USART_IsActiveFlag_TXE(USART2) == RESET);
			LL_USART_TransmitData8(USART2, str[i]);
			++i;
	}
}
