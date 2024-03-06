/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	led.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2016-11-23
	*
	*	版本： 		V1.0
	*
	*	说明： 		LED初始化，亮灭LED
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//LED头文件
#include "led.h"
#include "hwtimer.h"



LED_STATUS led_status;


/*
************************************************************
*	函数名称：	Led_Init
*
*	函数功能：	LED初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		LED4-PC7	LED5-PC8	LED6-PA12	LED7-PC10
				高电平关灯		低电平开灯
************************************************************
*/
void Led_Init(void)
{
	
	GPIO_InitTypeDef gpioInitStrcut;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	
	gpioInitStrcut.GPIO_Mode = GPIO_Mode_Out_PP;
	gpioInitStrcut.GPIO_Pin = GPIO_Pin_10;
	gpioInitStrcut.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpioInitStrcut);
	
	gpioInitStrcut.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOA, &gpioInitStrcut);
	
	Timer1_8_Init(TIM8, 250, 71);
    
    Led4_Set(0);
    Led5_Set(0);
    Led6_Set(LED_OFF);
    Led7_Set(LED_OFF);

}

/*
************************************************************
*	函数名称：	Led4_Set
*
*	函数功能：	LED4控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led4_Set(unsigned char pwmValue)
{

	TIM_SetCompare2(TIM8, pwmValue);
	led_status.Led4Sta = pwmValue;

}

/*
************************************************************
*	函数名称：	Led5_Set
*
*	函数功能：	LED5控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led5_Set(unsigned char pwmValue)
{

	TIM_SetCompare3(TIM8, pwmValue);
	led_status.Led5Sta = pwmValue;

}

/*
************************************************************
*	函数名称：	Led6_Set
*
*	函数功能：	LED6控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led6_Set(LED_ENUM status)
{

	GPIO_WriteBit(GPIOA, GPIO_Pin_12, status != LED_ON ? Bit_SET : Bit_RESET);
	led_status.Led6Sta = status;

}

/*
************************************************************
*	函数名称：	Led7_Set
*
*	函数功能：	LED7控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led7_Set(LED_ENUM status)
{

	GPIO_WriteBit(GPIOC, GPIO_Pin_10, status != LED_ON ? Bit_SET : Bit_RESET);
	led_status.Led7Sta = status;

}
