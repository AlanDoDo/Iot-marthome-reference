/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	hwtimer.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		��Ƭ����ʱ����ʼ��
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//Ӳ������
#include "hwtimer.h"


TIM_INFO timer_info = {0};


/*
************************************************************
*	�������ƣ�	Timer1_8_Init
*
*	�������ܣ�	Timer1��8��PWM����
*
*	��ڲ�����	TIMx��TIM1 ���� TIM8
*				arr������ֵ
*				psc��Ƶֵ
*
*	���ز�����	��
*
*	˵����		
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
	
	timer_oc_initstruct.TIM_OCMode = TIM_OCMode_PWM1;				//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
 	timer_oc_initstruct.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	timer_oc_initstruct.TIM_OCPolarity = TIM_OCPolarity_Low;		//�������:TIM����Ƚϼ��Ե�
	timer_oc_initstruct.TIM_Pulse = 0;
	TIM_OC2Init(TIMx, &timer_oc_initstruct);
	TIM_OC3Init(TIMx, &timer_oc_initstruct);
	
	TIM_CtrlPWMOutputs(TIMx, ENABLE);							//MOE �����ʹ��	
	
	TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);			//ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
	TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);			//ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���
 
	TIM_ARRPreloadConfig(TIMx, ENABLE);							//ARPEʹ��
	
	TIM_Cmd(TIMx, ENABLE);										//ʹ��TIMx

}

/*
************************************************************
*	�������ƣ�	Timer6_7_Init
*
*	�������ܣ�	Timer6��7�Ķ�ʱ����
*
*	��ڲ�����	TIMx��TIM6 ���� TIM7
*				arr������ֵ
*				psc��Ƶֵ
*
*	���ز�����	��
*
*	˵����		timer6��timer7ֻ���и����жϹ���
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
	
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);					//ʹ�ܸ����ж�
	
	nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;
	nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 1;
	nvic_initstruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&nvic_initstruct);
	
	TIM_Cmd(TIMx, ENABLE); //ʹ�ܶ�ʱ��

}

/*
************************************************************
*	�������ƣ�	TIM6_IRQHandler
*
*	�������ܣ�	RTOS��������ʱ�ж�
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
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
*	�������ƣ�	TIM7_IRQHandler
*
*	�������ܣ�	Timer7�����жϷ�����
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void TIM7_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM7, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
	}

}
