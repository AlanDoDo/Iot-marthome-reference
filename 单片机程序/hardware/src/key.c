//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//Ӳ������
#include "key.h"
#include "delay.h"
#include "led.h"

/*
************************************************************
*	�������ƣ�	Key_Init
*
*	�������ܣ�	Key��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/

void Key_Init(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    EXTI_InitTypeDef exti_initstruct;
    NVIC_InitTypeDef nvic_initstruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);     //��GPIOA��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IPU;                //����Ϊ��������ģʽ
    gpio_initstruct.GPIO_Pin = GPIO_Pin_1;                    //����ʼ����Pin����ΪGPIO_Pin_1����PA1����
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;            //����IO�ڵ������ٶ�Ϊ50MHz
    GPIO_Init(GPIOA, &gpio_initstruct);                       //��ʼ��GPIOA
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);    //��GPIOA��Pin1���ӵ��ⲿ�ж���1
    
    exti_initstruct.EXTI_Line = EXTI_Line1;                   //ѡ���ⲿ�ж���1
    exti_initstruct.EXTI_Mode = EXTI_Mode_Interrupt;          //����Ϊ�ж�ģʽ
    exti_initstruct.EXTI_Trigger = EXTI_Trigger_Falling;      //������ʽΪ�½���
    exti_initstruct.EXTI_LineCmd = ENABLE;                    //ʹ���ⲿ�ж���1
    EXTI_Init(&exti_initstruct);                              //��ʼ���ⲿ�ж�
    
    nvic_initstruct.NVIC_IRQChannel = EXTI1_IRQn;             //�����ⲿ�ж�1���ж�ͨ��
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 2;    //��ռ���ȼ�Ϊ2
    nvic_initstruct.NVIC_IRQChannelSubPriority = 2;           //�����ȼ�Ϊ2
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;              //ʹ���ⲿ�ж�1���ж�
    NVIC_Init(&nvic_initstruct);                              //��ʼ���ⲿ�ж��ж�������
}

void EXTI1_IRQHandler(void)
{
    DelayXms(10);   //��ʱ10ms
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)  //���PA1����Ϊ�͵�ƽ���������£�
    {
        if(led_info.Led_Status == LED_ON)
            Led_Set(LED_OFF);   //���LED״̬Ϊ������LED�ر�
        else
            Led_Set(LED_ON);    //���LED״̬Ϊ�أ���LED��
    }
    EXTI_ClearITPendingBit(EXTI_Line1);  //����ⲿ�ж���1���жϱ�־λ
}

