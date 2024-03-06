/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
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
#include "tcpkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
#include "adxl362.h"

//C库
#include <string.h>
#include <stdio.h>


#define PROID		"187421"

#define AUTH_INFO	"test"

#define LUA_NAME	"tcp362"


//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	TCP_PACKET_STRUCTURE tcpPacket = {NULL, 0, 0, 0};				//协议包

	unsigned char *dataPtr;
	
	unsigned char status = 1;
	
	UsartPrintf(USART_DEBUG, "OneNET_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	LUA:%s\r\n"
                        , PROID, AUTH_INFO, LUA_NAME);

	if(TCP_PacketConnect(PROID, AUTH_INFO, LUA_NAME, &tcpPacket) == 0)
	{
		M6312_SendData(tcpPacket._data, tcpPacket._len);			//上传平台
		
		dataPtr = M6312_GetIPD(250);								//等待平台响应
		if(dataPtr != NULL)
		{
			if(TCP_UnPacketRecv(dataPtr) == TCP_CONNECT)
			{
				UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");
				status = 0;
			}
		}
		
		TCP_DeleteBuffer(&tcpPacket);								//删包
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	TCP_PacketConnect Failed\r\n");
	
	return status;
	
}

void OneNet_FillBuf(unsigned char *buf, unsigned char offset)
{
	
	unsigned int temp = 0;
	unsigned char *data_ptr = buf + offset;
	
	temp = adxl362Info.x * 100;
	*data_ptr++ = temp >> 24;
	*data_ptr++ = temp >> 16;
	*data_ptr++ = temp >> 8;
	*data_ptr++ = temp  & 0x00FF;
	
	temp = adxl362Info.y * 100;
	*data_ptr++ = temp >> 24;
	*data_ptr++ = temp >> 16;
	*data_ptr++ = temp >> 8;
	*data_ptr++ = temp  & 0x00FF;
	
	temp = adxl362Info.z * 100;
	*data_ptr++ = temp >> 24;
	*data_ptr++ = temp >> 16;
	*data_ptr++ = temp >> 8;
	*data_ptr++ = temp  & 0x00FF;

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
	
	TCP_PACKET_STRUCTURE tcpPacket = {NULL, 0, 0, 0};											//协议包
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-TCP\r\n");
	
	if(TCP_PacketData(12, &tcpPacket) == 0)
	{
		OneNet_FillBuf(tcpPacket._data, 3);
		tcpPacket._len = 17;
		
		M6312_SendData(tcpPacket._data, tcpPacket._len);										//上传数据到平台
		UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", tcpPacket._len);
		
		TCP_DeleteBuffer(&tcpPacket);															//删包
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	
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
void OneNet_RevPro(unsigned char *cmd)
{
	
	M6312_Clear();										//清空缓存

}
