/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	hwtimer.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2016-11-23
	*
	*	版本： 		V1.0
	*
	*	说明： 		单片机定时器初始化
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//硬件驱动
#include "hwtimer.h"


TIM_INFO timer_info = {0};


/*
************************************************************
*	函数名称：	Timer1_8_Init
*
*	函数功能：	Timer1或8的PWM配置
*
*	入口参数：	TIMx：TIM1 或者 TIM8
*				arr：重载值
*				psc分频值
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Timer1_8_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc)
{
	
	GPIO_InitTypeDef gpio_initstruct;
	TIM_TimeBaseInitTypeDef timer_initstruct;
	TIM_OCInitTypeDef timer_oc_initstruct;

	if(TIMx == TIM1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	}
	else
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	}
	
	gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_initstruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpio_initstruct);	
	
	timer_initstruct.TIM_ClockDivision = TIM_CKD_DIV1;
	timer_initstruct.TIM_CounterMode = TIM_CounterMode_Up;
	timer_initstruct.TIM_Period = arr;
	timer_initstruct.TIM_Prescaler = psc;
	TIM_TimeBaseInit(TIMx, &timer_initstruct);
	
	timer_oc_initstruct.TIM_OCMode = TIM_OCMode_PWM1;				//选择定时器模式:TIM脉冲宽度调制模式1
 	timer_oc_initstruct.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	timer_oc_initstruct.TIM_OCPolarity = TIM_OCPolarity_Low;		//输出极性:TIM输出比较极性低
	timer_oc_initstruct.TIM_Pulse = 0;
	TIM_OC2Init(TIMx, &timer_oc_initstruct);
	TIM_OC3Init(TIMx, &timer_oc_initstruct);
	
	TIM_CtrlPWMOutputs(TIMx, ENABLE);							//MOE 主输出使能	
	
	TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);			//使能TIMx在CCR1上的预装载寄存器
	TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);			//使能TIMx在CCR1上的预装载寄存器
 
	TIM_ARRPreloadConfig(TIMx, ENABLE);							//ARPE使能
	
	TIM_Cmd(TIMx, ENABLE);										//使能TIMx

}

/*
************************************************************
*	函数名称：	Timer6_7_Init
*
*	函数功能：	Timer6或7的定时配置
*
*	入口参数：	TIMx：TIM6 或者 TIM7
*				arr：重载值
*				psc分频值
*
*	返回参数：	无
*
*	说明：		timer6和timer7只具有更新中断功能
************************************************************
*/
void Timer6_7_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc)
{

	TIM_TimeBaseInitTypeDef timer_initstruct;
	NVIC_InitTypeDef nvic_initstruct;
	
	if(TIMx == TIM6)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
		
		nvic_initstruct.NVIC_IRQChannel = TIM6_IRQn;
	}
	else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
		
		nvic_initstruct.NVIC_IRQChannel = TIM7_IRQn;
	}
	
	timer_initstruct.TIM_CounterMode = TIM_CounterMode_Up;
	timer_initstruct.TIM_Period = arr;
	timer_initstruct.TIM_Prescaler = psc;
	
	TIM_TimeBaseInit(TIMx, &timer_initstruct);
	
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);					//使能更新中断
	
	nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
	nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_initstruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&nvic_initstruct);
	
	TIM_Cmd(TIMx, ENABLE); //使能定时器

}

/*
************************************************************
*	函数名称：	TIM6_IRQHandler
*
*	函数功能：	RTOS的心跳定时中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void TIM6_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM6, TIM_IT_Update) == SET)
	{
		//do something...
		
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
	}

}

/*
************************************************************
*	函数名称：	TIM7_IRQHandler
*
*	函数功能：	Timer7更新中断服务函数
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void TIM7_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM7, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	}

}
