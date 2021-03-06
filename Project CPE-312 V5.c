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
#include "stm32l1xx_ll_tim.h"
#include "String.h"

#define C							(uint16_t)1046
#define D							(uint16_t)1174
#define E							(uint16_t)1318
#define F							(uint16_t)1396
#define G							(uint16_t)1568
#define A							(uint16_t)1760
#define B							(uint16_t)1975
#define P							(uint16_t) 1
#define TIMx_PSC			2
#define MAX_LENGTH		7
#define ARR_CALCULATE(N) ((32000000) / ((TIMx_PSC) * (N)))

void SystemClock_Config(void);
void GPIO_USART_Configure(void);
void USART_Configure(void);
void USART_SendString(uint8_t*, uint8_t);
uint8_t recv_beffer[MAX_LENGTH];
uint8_t idx = 0;
int j = 0;

void TIM_BASE_Config(uint16_t);
void TIM_OC_GPIO_Config(void);
void TIM_OC_Config(uint16_t);
void TIM_BASE_DurationConfig(void);	
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);	
void set_rgb(uint8_t red, uint8_t green, uint8_t blue);

	
int main()
{
	SystemClock_Config();
	USART_Configure();
	TIM_BASE_DurationConfig();
	MX_GPIO_Init();
  MX_TIM3_Init();
	
	LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH2);
	LL_TIM_CC_EnableChannel(TIM3, LL_TIM_CHANNEL_CH3);
	
	while(1)
	{
		while(LL_USART_IsActiveFlag_RXNE(USART2) == RESET);
		recv_beffer[idx++] = LL_USART_ReceiveData8(USART2);
		idx = idx % MAX_LENGTH;
		
		if(strstr((const char*)recv_beffer,"\r\n") != NULL)
		{
				while(j < MAX_LENGTH)
				{
					if(strstr((const char*)recv_beffer,"C") != NULL)
					{
						TIM_OC_Config(ARR_CALCULATE(recv_beffer[j]));
						LL_TIM_ClearFlag_UPDATE(TIM2);
						set_rgb(255,0,0);
						LL_mDelay(2000);
					}
					else if(strstr((const char*)recv_beffer,"D") != NULL)
					{
						TIM_OC_Config(ARR_CALCULATE(recv_beffer[j]));
						LL_TIM_ClearFlag_UPDATE(TIM2);
						set_rgb(0,255,0);
						LL_mDelay(2000);
					}
					else if(strstr((const char*)recv_beffer,"E") != NULL)
					{
						TIM_OC_Config(ARR_CALCULATE(recv_beffer[j]));
						LL_TIM_ClearFlag_UPDATE(TIM2);
						set_rgb(0,0,255);
						LL_mDelay(2000);
					}
					else if(strstr((const char*)recv_beffer,"F") != NULL)
					{
						TIM_OC_Config(ARR_CALCULATE(recv_beffer[j]));
						LL_TIM_ClearFlag_UPDATE(TIM2);
						set_rgb(192,192,192);
						LL_mDelay(2000);
					}
					else if(strstr((const char*)recv_beffer,"G") != NULL)
					{
						TIM_OC_Config(ARR_CALCULATE(recv_beffer[j]));
						LL_TIM_ClearFlag_UPDATE(TIM2);
						set_rgb(255,255,0);
						LL_mDelay(2000);
					}
					else if(strstr((const char*)recv_beffer,"A") != NULL)
					{
						TIM_OC_Config(ARR_CALCULATE(recv_beffer[j]));
						LL_TIM_ClearFlag_UPDATE(TIM2);
						set_rgb(0,255,255);
						LL_mDelay(2000);
					}
					else if(strstr((const char*)recv_beffer,"B") != NULL)
					{
						TIM_OC_Config(ARR_CALCULATE(recv_beffer[j]));
						LL_TIM_ClearFlag_UPDATE(TIM2);
						set_rgb(255,0,255);
						LL_mDelay(2000);
					}
					j++;
					recv_beffer[0] = recv_beffer[j];
					recv_beffer[j] = 0;
				}
				
				set_rgb(192,192,192);
				LL_TIM_SetAutoReload(TIM4, P);
				memset(recv_beffer, NULL, MAX_LENGTH);
				idx = 0;
		}
	}
}
void set_rgb(uint8_t red, uint8_t green, uint8_t blue)
{
	TIM3->CCR1 = red;
	TIM3->CCR2 = green;
	TIM3->CCR3 = blue;
}


static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};
  LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  TIM_InitStruct.Prescaler = 1384 - 1;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 255 - 1;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM3, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM3);
  LL_TIM_SetClockSource(TIM3, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH1);
  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 0;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM3, LL_TIM_CHANNEL_CH1);
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH2);
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH2, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM3, LL_TIM_CHANNEL_CH2);
  LL_TIM_OC_EnablePreload(TIM3, LL_TIM_CHANNEL_CH3);
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  LL_TIM_OC_Init(TIM3, LL_TIM_CHANNEL_CH3, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(TIM3, LL_TIM_CHANNEL_CH3);
  LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM3);
  /* USER CODE BEGIN TIM3_Init 2 */
	LL_TIM_EnableCounter(TIM3);

  /* USER CODE END TIM3_Init 2 */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
  /**TIM3 GPIO Configuration
  PC6   ------> TIM3_CH1
  PC7   ------> TIM3_CH2
  PC8   ------> TIM3_CH3
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_8;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_2;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

}

void TIM_BASE_DurationConfig(void)
{
	LL_TIM_InitTypeDef timbase_initstructure;
	
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	//Time-base configure
	timbase_initstructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	timbase_initstructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	timbase_initstructure.Autoreload = 800 - 1;
	timbase_initstructure.Prescaler =  32000 - 1;
	LL_TIM_Init(TIM2, &timbase_initstructure);
	
	LL_TIM_EnableCounter(TIM2); 
	LL_TIM_ClearFlag_UPDATE(TIM2); //Force clear update flag
}

void TIM_BASE_Config(uint16_t ARR)
{
	LL_TIM_InitTypeDef timbase_initstructure;
	
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
	//Time-base configure
	timbase_initstructure.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
	timbase_initstructure.CounterMode = LL_TIM_COUNTERMODE_UP;
	timbase_initstructure.Autoreload = ARR - 1;
	timbase_initstructure.Prescaler =  TIMx_PSC- 1;
	LL_TIM_Init(TIM4, &timbase_initstructure);
	
	LL_TIM_EnableCounter(TIM4); 
}


void TIM_OC_GPIO_Config(void)
{
	LL_GPIO_InitTypeDef gpio_initstructure;
	
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	
	gpio_initstructure.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio_initstructure.Alternate = LL_GPIO_AF_2;
	gpio_initstructure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_initstructure.Pin = LL_GPIO_PIN_6;
	gpio_initstructure.Pull = LL_GPIO_PULL_NO;
	gpio_initstructure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	LL_GPIO_Init(GPIOB, &gpio_initstructure);
}

void TIM_OC_Config(uint16_t note)
{
	LL_TIM_OC_InitTypeDef tim_oc_initstructure;
	
	TIM_OC_GPIO_Config();
	TIM_BASE_Config(note);
	
	tim_oc_initstructure.OCState = LL_TIM_OCSTATE_DISABLE;
	tim_oc_initstructure.OCMode = LL_TIM_OCMODE_PWM1;
	tim_oc_initstructure.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
	tim_oc_initstructure.CompareValue = LL_TIM_GetAutoReload(TIM4) / 2;
	LL_TIM_OC_Init(TIM4, LL_TIM_CHANNEL_CH1, &tim_oc_initstructure);
	/*Interrupt Configure*/
	NVIC_SetPriority(TIM4_IRQn, 1);
	NVIC_EnableIRQ(TIM4_IRQn);
	LL_TIM_EnableIT_CC1(TIM4);
	
	/*Start Output Compare in PWM Mode*/
	LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableCounter(TIM4);
}

void TIM4_IRQHandler(void)
{
	if(LL_TIM_IsActiveFlag_CC1(TIM4) == SET)
	{
		LL_TIM_ClearFlag_CC1(TIM4);
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

void SystemClock_Config(void)
{
  /* Enable ACC64 access and set FLASH latency */ 
  LL_FLASH_Enable64bitAccess();; 
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

  /* Set Voltage scale1 as MCU will run at 32MHz */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  
  /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
  while (LL_PWR_IsActiveFlag_VOSF() != 0)
  {
  };
  
  /* Enable HSI if not already activated*/
  if (LL_RCC_HSI_IsReady() == 0)
  {
    /* HSI configuration and activation */
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1)
    {
    };
  }
  
	
  /* Main PLL configuration and activation */
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3);

  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  };
  
  /* Sysclk activation on the main PLL */
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };
  
  /* Set APB1 & APB2 prescaler*/
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  /* Set systick to 1ms in using frequency set to 32MHz                             */
  /* This frequency can be calculated through LL RCC macro                          */
  /* ex: __LL_RCC_CALC_PLLCLK_FREQ (HSI_VALUE, LL_RCC_PLL_MUL_6, LL_RCC_PLL_DIV_3); */
  LL_Init1msTick(32000000);
  
  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(32000000);
}