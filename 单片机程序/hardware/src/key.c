
//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "key.h"
#include "delay.h"
#include "led.h"

/*
************************************************************
*	函数名称：	Led_Init
*
*	函数功能：	蜂鸣器初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Key_Init(void)
{

	GPIO_InitTypeDef gpio_initstruct;
	EXTI_InitTypeDef exti_initstruct;
	NVIC_InitTypeDef nvic_initstruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		//打开GPIOB的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	gpio_initstruct.GPIO_Mode = GPIO_Mode_IPU;				//设置为输出
	gpio_initstruct.GPIO_Pin = GPIO_Pin_1;						//将初始化的Pin脚
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;				//可承载的最大频率
	GPIO_Init(GPIOA, &gpio_initstruct);							//初始化GPIO
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
	
	exti_initstruct.EXTI_Line = EXTI_Line1;
	exti_initstruct.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_initstruct.EXTI_Trigger = EXTI_Trigger_Falling;
	exti_initstruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_initstruct);
	
	nvic_initstruct.NVIC_IRQChannel = EXTI1_IRQn;
	nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 2;
	nvic_initstruct.NVIC_IRQChannelSubPriority = 2;
	nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_initstruct);
	
}

void EXTI1_IRQHandler(void)
{
	DelayXms(10);
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)==0)
	{
		if(led_info.Led_Status==LED_ON) Led_Set(LED_OFF);
		else Led_Set(LED_ON);
	}
	EXTI_ClearITPendingBit(EXTI_Line1);
}
