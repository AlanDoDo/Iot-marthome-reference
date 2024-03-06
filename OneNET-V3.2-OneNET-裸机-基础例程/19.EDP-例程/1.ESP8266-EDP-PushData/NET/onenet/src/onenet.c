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
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "edpkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"

//C库
#include <string.h>
#include <stdio.h>


#define DEVID	"6580246"

#define APIKEY	"iUZqKW6xIjpGngggBH=66VCGzqg="


extern unsigned char esp8266_buf[128];


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
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};				//协议包

	unsigned char *dataPtr;
	
	unsigned char status = 1;
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                        "DEVID: %s,     APIKEY: %s\r\n"
                        , DEVID, APIKEY);

	if(EDP_PacketConnect1(DEVID, APIKEY, 256, &edpPacket) == 0)		//根据devid 和 apikey封装协议包
	{
		ESP8266_SendData(edpPacket._data, edpPacket._len);			//上传平台
		
		dataPtr = ESP8266_GetIPD(250);								//等待平台响应
		if(dataPtr != NULL)
		{
			if(EDP_UnPacketRecv(dataPtr) == CONNRESP)
			{
				switch(EDP_UnPacketConnectRsp(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：设备ID鉴权失败\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户ID鉴权失败\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：未授权\r\n");break;
					case 6:UsartPrintf(USART_DEBUG, "WARN:	连接失败：授权码无效\r\n");break;
					case 7:UsartPrintf(USART_DEBUG, "WARN:	连接失败：激活码未分配\r\n");break;
					case 8:UsartPrintf(USART_DEBUG, "WARN:	连接失败：该设备已被激活\r\n");break;
					case 9:UsartPrintf(USART_DEBUG, "WARN:	连接失败：重复发送连接请求包\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		EDP_DeleteBuffer(&edpPacket);								//删包
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	EDP_PacketConnect Failed\r\n");
	
	return status;
	
}

//==========================================================
//	函数名称：	OneNet_PushData
//
//	函数功能：	PUSHDATA
//
//	入口参数：	dst_devid：接收设备的devid
//				data：数据内容
//				data_len：数据长度
//
//	返回参数：	0-发送成功	1-失败
//
//	说明：		设备与设备之间的通信
//==========================================================
_Bool OneNet_PushData(const char* dst_devid, const char* data, unsigned int data_len)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};							//协议包
	
	if(EDP_PacketPushData(dst_devid, data, data_len, &edpPacket) == 0)
	{
		UsartPrintf(USART_DEBUG, "Tips:	OneNet_PushData\r\n");
		
		ESP8266_SendData(edpPacket._data, edpPacket._len);						//上传平台
		
		EDP_DeleteBuffer(&edpPacket);											//删包
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	OneNet_PushData Failed\r\n");
	
	return 0;

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
	
	char *cmdid_devid = NULL;
	char *req = NULL;
	unsigned int req_len = 0;
	unsigned char type = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	type = EDP_UnPacketRecv(cmd);
	switch(type)										//判断是pushdata还是命令下发
	{
		case PUSHDATA:									//解pushdata包
			
			result = EDP_UnPacketPushData(cmd, &cmdid_devid, &req, &req_len);
		
			if(result == 0)
				UsartPrintf(USART_DEBUG, "src_devid: %s, req: %s, req_len: %d\r\n", cmdid_devid, req, req_len);
			
		break;
			
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req, '}');							//搜索'}'
	
	if(dataPtr != NULL)									//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//转为数值形式
	}
	
	if(type == PUSHDATA && result == 0)					//如果是pushdata 且 解包成功
	{
		EDP_FreeBuffer(cmdid_devid);					//释放内存
		EDP_FreeBuffer(req);
	}

}
