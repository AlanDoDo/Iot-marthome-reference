/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	m6312.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-10-20
	*
	*	版本： 		V1.0
	*
	*	说明： 		M6312驱动
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备驱动
#include "m6312.h"

//硬件驱动
#include "delay.h"
#include "usart.h"

//C库
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define M6312_ONENET_INFO		"AT+IPSTART=\"TCP\",\"183.230.40.42\",2987\r\n"


unsigned char m6312_buf[128];
unsigned short m6312_cnt = 0, m6312_cntPre = 0;
unsigned char data_bytes;


//==========================================================
//	函数名称：	M6312_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void M6312_Clear(void)
{

	memset(m6312_buf, 0, sizeof(m6312_buf));
	m6312_cnt = 0;

}

//==========================================================
//	函数名称：	M6312_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool M6312_WaitRecive(void)
{

	if(m6312_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(m6312_cnt == m6312_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		m6312_cnt = 0;							//清0接收计数
			
		return REV_OK;							//返回接收完成标志
	}
		
	m6312_cntPre = m6312_cnt;					//置为相同
	
	return REV_WAIT;							//返回接收未完成标志

}

//==========================================================
//	函数名称：	M6312_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool M6312_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(M6312_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)m6312_buf, res) != NULL)		//如果检索到关键词
			{
				M6312_Clear();										//清空缓存
				
				return 0;
			}
		}
		
		DelayXms(10);
	}
	
	return 1;

}

//==========================================================
//	函数名称：	M6312_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void M6312_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	M6312_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+IPSEND=%d\r\n", len);	//发送命令
	if(!M6312_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
	{
		Usart_SendString(USART2, data, len);	//发送设备连接请求数据
	}

}

//==========================================================
//	函数名称：	M6312_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	timeOut等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//==========================================================
unsigned char *M6312_GetIPD(unsigned short timeOut)
{

	char *ptrIPD = NULL;
	char bytes[4] = {0, 0, 0, 0};
	unsigned char i = 0;
	
	do
	{
		if(M6312_WaitRecive() == REV_OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)m6312_buf, "IPDATA:");				//搜索“IPDATA”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPDATA头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');					//找到':'
				
				while(*ptrIPD < '0' || *ptrIPD > '9')			//找到第一个为数字的字符
				{
					if(*ptrIPD == '\0')	return NULL;
					
					ptrIPD++;
				}
				while(*ptrIPD >= '0' && *ptrIPD <= '9')			//复制
					bytes[i++] = *ptrIPD++;
				
				data_bytes = atoi(bytes);					//转换
				
				while(*ptrIPD != '\n' && *ptrIPD != '\0')		//
					ptrIPD++;
				
				if(*ptrIPD != '\0')
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
			}
		}
		
		DelayXms(10);													//延时等待
	} while(timeOut--);
	
	return NULL;														//超时还未找到，返回空指针

}

//==========================================================
//	函数名称：	M6312_Init
//
//	函数功能：	初始化M6312
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void M6312_Init(void)
{
	
	GPIO_InitTypeDef GPIO_Initure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

	//M6312复位引脚
	GPIO_Initure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_15;				//GPIOC15-复位
	GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_Initure);
	
	//M6312电源引脚
	GPIO_Initure.GPIO_Pin = GPIO_Pin_4;					//GPIOC4-PowerKey
	GPIO_Init(GPIOC, &GPIO_Initure);
	
	//M6312状态引脚
	GPIO_Initure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Initure.GPIO_Pin = GPIO_Pin_5;					//GPIOC5-status
	GPIO_Init(GPIOC, &GPIO_Initure);
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_4, Bit_SET);			//上电
	
	while(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5))
		DelayXms(10);
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_15, Bit_SET);			//复位
	DelayXms(250);
	GPIO_WriteBit(GPIOC, GPIO_Pin_15, Bit_RESET);
	DelayXms(500);
	
	M6312_Clear();

	UsartPrintf(USART_DEBUG, "1. AT\r\n");
	while(M6312_SendCmd("AT\r\n", "OK"))
		DelayXms(500);
	
	UsartPrintf(USART_DEBUG, "2. AT+CPIN?\r\n");
	while(M6312_SendCmd("AT+CPIN?\r\n", "+CPIN: READY"))		//确保SIM卡PIN码解锁，返回READY，表示解锁成功
		DelayXms(500);
	
	UsartPrintf(USART_DEBUG, "3. AT+CREG?\r\n");
	while(M6312_SendCmd("AT+CREG?\r\n", "1,5"))
		DelayXms(500);
	
	UsartPrintf(USART_DEBUG, "4. AT+CGREG?\r\n");
	while(M6312_SendCmd("AT+CGREG?\r\n","OK"))					//检查网络注册状态
		DelayXms(500);
	
	UsartPrintf(USART_DEBUG, "5. AT+CGACT=1,1\r\n");
	while(M6312_SendCmd("AT+CGACT=1,1\r\n","OK"))				//激活
		DelayXms(500);
	
	UsartPrintf(USART_DEBUG, "6. AT+CGATT=1\r\n");
	while(M6312_SendCmd("AT+CGATT=1\r\n","OK"))					//附着GPRS业务
		DelayXms(500);
	
	UsartPrintf(USART_DEBUG, "7. AT+CMMUX=0\r\n");
	while(M6312_SendCmd("AT+CMMUX=0\r\n","OK"))					//必须为单连接，不然平台IP都连不上
		DelayXms(500);
	
	UsartPrintf(USART_DEBUG, "8. AT+CMHEAD=1\r\n");
	while(M6312_SendCmd("AT+CMHEAD=1\r\n","OK"))				//显示IP头
		DelayXms(500);
	
	UsartPrintf(USART_DEBUG, "9. AT+IPSTART\r\n");
	while(M6312_SendCmd(M6312_ONENET_INFO,"CONNECT"))				//
		DelayXms(500);
	
	UsartPrintf(USART_DEBUG, "10. M6312 Init OK\r\n");

}

//==========================================================
//	函数名称：	USART2_IRQHandler
//
//	函数功能：	串口2收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
	{
		if(m6312_cnt >= sizeof(m6312_buf))	m6312_cnt = 0; //防止串口被刷爆
		m6312_buf[m6312_cnt++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}

}
