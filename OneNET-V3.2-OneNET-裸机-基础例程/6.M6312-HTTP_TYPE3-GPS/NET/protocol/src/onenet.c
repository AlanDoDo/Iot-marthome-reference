/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-02-27
	*
	*	版本： 		V1.1
	*
	*	说明： 		与onenet平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "m6312.h"

//协议文件
#include "onenet.h"

//硬件驱动
#include "usart.h"
#include "delay.h"

//C库
#include <string.h>
#include <stdio.h>


#define DEVID	"5616708"

#define APIKEY	"kPl=kf99QxL2acVvVCHssUPWZKs="


float lon = 106.498636;
float lat = 29.629081;


void OneNet_FillBuf(char *buf)
{
	
	char text[64];
	char buf1[128];
	
	memset(text, 0, sizeof(text));
	memset(buf1, 0, sizeof(buf1));
	
	strcpy(buf1, "{");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"GPS\":{\"lon\":%f,\"lat\":%f}", lon, lat);
	strcat(buf1, text);
	
	strcat(buf1, "}");
	
	sprintf(buf, "POST /devices/%s/datapoints?type=3 HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\n"
					"Content-Length:%d\r\n\r\n",
	
					DEVID, APIKEY, strlen(buf1));
					
	strcat(buf, buf1);

}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(void)
{
	
	char buf[256];
	
	OneNet_FillBuf(buf);								//封装数据流
	
	M6312_SendData((unsigned char *)buf, strlen(buf));	//上传数据
	
}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_RevPro(unsigned char *dataPtr)
{

	if(strstr((char *)dataPtr, "CLOSED"))
	{
		UsartPrintf(USART_DEBUG, "TCP CLOSED\r\n");
	}
	else
	{
		//这里用来检测是否发送成功
		if(strstr((char *)dataPtr, "succ"))
		{
			UsartPrintf(USART_DEBUG, "Tips:	Send OK\r\n");
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Tips:	Send Err\r\n");
		}
	}
	
	M6312_Clear();

}
