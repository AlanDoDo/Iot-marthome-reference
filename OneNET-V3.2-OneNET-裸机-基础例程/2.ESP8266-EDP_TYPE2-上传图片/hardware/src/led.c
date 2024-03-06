/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	led.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		LED��ʼ��������LED
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//LEDͷ�ļ�
#include "led.h"


LED_STATUS led_status;


/*
************************************************************
*	�������ƣ�	Led_Init
*
*	�������ܣ�	LED��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		LED4-PC7	LED5-PC8	LED6-PA12	LED7-PC10
				�ߵ�ƽ�ص�		�͵�ƽ����
************************************************************
*/
void Led_Init(void)
{
	
	GPIO_InitTypeDef gpio_initstruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);	//��GPIOA��GPIOC��ʱ��
	
	gpio_initstruct.GPIO_Mode = GPIO_Mode_Out_PP;									//����Ϊ�������ģʽ
	gpio_initstruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 |GPIO_Pin_10;					//��ʼ��Pin7��8��10
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;									//���ص����Ƶ��
	GPIO_Init(GPIOC, &gpio_initstruct);												//��ʼ��GPIOC
	
	gpio_initstruct.GPIO_Pin = GPIO_Pin_12;											//��ʼ��Pin12
	GPIO_Init(GPIOA, &gpio_initstruct);												//��ʼ��GPIOA
    
    Led4_Set(LED_OFF);
    Led5_Set(LED_OFF);
    Led6_Set(LED_OFF);
    Led7_Set(LED_OFF);

}

/*
************************************************************
*	�������ƣ�	Led4_Set
*
*	�������ܣ�	LED4����
*
*	��ڲ�����	status��LED_ON-����	LED_OFF-�ص�
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Led4_Set(LED_ENUM status)
{

	GPIO_WriteBit(GPIOC, GPIO_Pin_7, status != LED_ON ? Bit_SET : Bit_RESET);	//status���������LED_ON�򷵻�Bit_SET�����򷵻�Bit_RESET����ͬ
	led_status.Led4Sta = status;

}

/*
************************************************************
*	�������ƣ�	Led5_Set
*
*	�������ܣ�	LED5����
*
*	��ڲ�����	status��LED_ON-����	LED_OFF-�ص�
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Led5_Set(LED_ENUM status)
{

	GPIO_WriteBit(GPIOC, GPIO_Pin_8, status != LED_ON ? Bit_SET : Bit_RESET);
	led_status.Led5Sta = status;

}

/*
************************************************************
*	�������ƣ�	Led6_Set
*
*	�������ܣ�	LED6����
*
*	��ڲ�����	status��LED_ON-����	LED_OFF-�ص�
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Led6_Set(LED_ENUM status)
{

	GPIO_WriteBit(GPIOA, GPIO_Pin_12, status != LED_ON ? Bit_SET : Bit_RESET);
	led_status.Led6Sta = status;

}

/*
************************************************************
*	�������ƣ�	Led7_Set
*
*	�������ܣ�	LED7����
*
*	��ڲ�����	status��LED_ON-����	LED_OFF-�ص�
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Led7_Set(LED_ENUM status)
{

	GPIO_WriteBit(GPIOC, GPIO_Pin_10, status != LED_ON ? Bit_SET : Bit_RESET);
	led_status.Led7Sta = status;

}
