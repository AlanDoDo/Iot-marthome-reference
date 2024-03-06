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
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "httpkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"

//C库
#include <string.h>
#include <stdio.h>


#define DEVID	"5616708"

#define APIKEY	"kPl=kf99QxL2acVvVCHssUPWZKs="


//==========================================================
//	函数名称：	OneNet_RegisterDevice
//
//	函数功能：	注册设备
//
//	入口参数：	apikey：master-key(产品APIKEY)
//				title：设备名
//				auth_info:设备的鉴权信息
//				desc：描述信息
//				Private：是否公开
//
//	返回参数：	无
//
//	说明：		非HTTP协议的设备，调用此函数前需要断开当前连接，再连接HTTP协议的ip
//==========================================================
void OneNet_RegisterDevice(const char *apikey, const char *title, const char *auth_info, const char *desc, const char *Private)
{
	
	HTTP_PACKET_STRUCTURE httpPacket = {NULL, 0, 0, 0};							//协议包
	
	if(HTTP_Post_PacketDeviceRegister(apikey, title, auth_info, desc, Private, &httpPacket) == 0)
	{
		ESP8266_SendData(httpPacket._data, httpPacket._len);				//上传数据到平台
		
		HTTP_DeleteBuffer(&httpPacket);										//删包
	}

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
			
			if(strstr((char *)dataPtr, "device_id"))
			{
				char *devid = NULL;
				
				UsartPrintf(USART_DEBUG, "\r\n%s\r\n", dataPtr);
				if(HTTP_UnPacketDeviceRegister(dataPtr, &devid) == 0)
				{
					UsartPrintf(USART_DEBUG, "devid: %s\r\n", devid);
					HTTP_FreeBuffer(devid);
				}
			}
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Tips:	Send Err\r\n");
		}
	}
	
	ESP8266_Clear();

}
