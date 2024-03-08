//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "key.h"
#include "delay.h"
#include "led.h"

/*
************************************************************
*	函数名称：	Key_Init
*
*	函数功能：	Key初始化
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
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);     //打开GPIOA的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IPU;                //设置为输入上拉模式
    gpio_initstruct.GPIO_Pin = GPIO_Pin_1;                    //将初始化的Pin脚设为GPIO_Pin_1，即PA1引脚
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;            //设置IO口的输入速度为50MHz
    GPIO_Init(GPIOA, &gpio_initstruct);                       //初始化GPIOA
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);    //将GPIOA的Pin1连接到外部中断线1
    
    exti_initstruct.EXTI_Line = EXTI_Line1;                   //选择外部中断线1
    exti_initstruct.EXTI_Mode = EXTI_Mode_Interrupt;          //设置为中断模式
    exti_initstruct.EXTI_Trigger = EXTI_Trigger_Falling;      //触发方式为下降沿
    exti_initstruct.EXTI_LineCmd = ENABLE;                    //使能外部中断线1
    EXTI_Init(&exti_initstruct);                              //初始化外部中断
    
    nvic_initstruct.NVIC_IRQChannel = EXTI1_IRQn;             //配置外部中断1的中断通道
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 2;    //抢占优先级为2
    nvic_initstruct.NVIC_IRQChannelSubPriority = 2;           //子优先级为2
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;              //使能外部中断1的中断
    NVIC_Init(&nvic_initstruct);                              //初始化外部中断中断向量表
}

void EXTI1_IRQHandler(void)
{
    DelayXms(10);   //延时10ms
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)  //如果PA1引脚为低电平（按键按下）
    {
        if(led_info.Led_Status == LED_ON)
            Led_Set(LED_OFF);   //如果LED状态为开，则将LED关闭
        else
            Led_Set(LED_ON);    //如果LED状态为关，则将LED打开
    }
    EXTI_ClearITPendingBit(EXTI_Line1);  //清除外部中断线1的中断标志位
}

