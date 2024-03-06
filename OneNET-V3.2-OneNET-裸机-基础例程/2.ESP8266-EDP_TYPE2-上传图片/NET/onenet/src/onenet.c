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
#include "led.h"

//图片
#include "image_2k.h"

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

unsigned char OneNet_FillBuf(char *buf)
{
	
	char text[16];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Red_Led\":%d,", led_status.Led4Sta);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Green_Led\":%d,", led_status.Led5Sta);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Yellow_Led\":%d,", led_status.Led6Sta);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"Blue_Led\":%d", led_status.Led7Sta);
	strcat(buf, text);
	
	strcat(buf, "}");
	
	return strlen(buf);

}

//==========================================================
//	函数名称：	OneNet_SendData_EDPType2
//
//	函数功能：	上传二进制数据到平台
//
//	入口参数：	devid：设备ID(推荐为NULL)
//				picture：图片数据
//				pic_len：图片数据长度
//
//	返回参数：	无
//
//	说明：		若是低速设备，数据量大时，建议使用网络设备的透传模式
//				上传图片是，强烈建议devid字段为空，否则平台会将图片数据下发到设备
//==========================================================
#define PKT_SIZE 1024
void OneNet_SendData_Picture(char *devid, const char* picture, unsigned int pic_len)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};					//协议包

	char type_bin_head[] = "{\"ds_id\":\"pic\"}";						//图片数据头
	unsigned char *pImage = (unsigned char *)picture;
	
	if(EDP_PacketSaveData(devid, pic_len, type_bin_head, kTypeBin, &edpPacket) == 0)
	{	
		ESP8266_Clear();
		
		UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", edpPacket._len);
		ESP8266_SendData(edpPacket._data, edpPacket._len);				//上传数据到平台
		
		EDP_DeleteBuffer(&edpPacket);									//删包
		
		UsartPrintf(USART_DEBUG, "image len = %d\r\n", pic_len);
		
		while(pic_len > 0)
		{
			DelayXms(300);												//传图时，时间间隔会大一点，这里额外增加一个延时
			
			if(pic_len >= PKT_SIZE)
			{
				ESP8266_SendData(pImage, PKT_SIZE);						//串口发送分片
				
				pImage += PKT_SIZE;
				pic_len -= PKT_SIZE;
			}
			else
			{
				ESP8266_SendData(pImage, (unsigned short)pic_len);		//串口发送最后一个分片
				pic_len = 0;
			}
		}
		
		UsartPrintf(USART_DEBUG, "image send ok\r\n");
	}
	else
		UsartPrintf(USART_DEBUG, "EDP_PacketSaveData Failed\r\n");

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
	
	OneNet_SendData_Picture(NULL, Array, sizeof(Array));
	
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
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};	//协议包
	
	char *cmdid_devid = NULL;
	char *req = NULL;
	unsigned char type = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	type = EDP_UnPacketRecv(cmd);
	switch(type)										//判断是pushdata还是命令下发
	{
		case SAVEACK:
			
			if(cmd[3] == MSG_ID_HIGH && cmd[4] == MSG_ID_LOW)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Send %s\r\n", cmd[5] ? "Err" : "Ok");
			}
			else
				UsartPrintf(USART_DEBUG, "Tips:	Message ID Err\r\n");
			
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
		numBuf[num] = 0;
		
		num = atoi((const char *)numBuf);				//转为数值形式
		
		
	}
	
	if(type == CMDREQ && result == 0)						//如果是命令包 且 解包成功
	{
		EDP_FreeBuffer(cmdid_devid);						//释放内存
		EDP_FreeBuffer(req);
															//回复命令
		ESP8266_SendData(edpPacket._data, edpPacket._len);	//上传平台
		EDP_DeleteBuffer(&edpPacket);						//删包
	}

}
