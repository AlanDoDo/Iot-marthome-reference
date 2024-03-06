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
#include "modbuskit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"

//C库
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define PROID	"113237"

#define SERIAL	"1234567"

#define PSWD	"1234567"


#define MODBUS_SLAVE_ADDR		1


unsigned char s_addr = 0, m_cmd = 0, rev_cmd_cnt = 0;
unsigned short r_addr = 0, r_len = 0;


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
	
	MODBUS_PACKET_STRUCTURE modbusPacket = {NULL, 0, 0, 0};				//协议包
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\nPROID: %s,	SERIAL: %s,	PSWD: %s\r\n"
								, PROID, SERIAL, PSWD);
	
	if(MODBUS_Connect(SERIAL, PSWD, PROID, &modbusPacket) == 0)
	{
		ESP8266_SendData(modbusPacket._data, modbusPacket._len);		//上传平台
		
		MODBUS_DeleteBuffer(&modbusPacket);								//删包
		
		s_addr = MODBUS_SLAVE_ADDR;
		
		return 0;
	}
	else
		return 1;
	
}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	buf：需要发送的数据
//				dataLen：数据长度(半字个数)
//
//	返回参数：	0-成功	其他-失败
//
//	说明：		
//==========================================================
unsigned char OneNet_SendData(unsigned short *buf ,unsigned short dataLen)
{
	
	MODBUS_PACKET_STRUCTURE modbusPacket = {NULL, 0, 0, 0};				//协议包
	
	if(MODBUS_PacketCmd(s_addr, m_cmd, buf, dataLen, &modbusPacket) == 0)
	{
		UsartPrintf(USART_DEBUG, "OneNet_SendData %d Bytes\r\n", modbusPacket._len);
		ESP8266_SendData(modbusPacket._data, modbusPacket._len);
	}
	
	return 1;

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
void OneNet_RevPro(unsigned char *cmd, unsigned short len)
{
	
	unsigned char ls_addr = 0;
	
	//平台手动命令下发格式：010100010001AC0A
	//注：命令体只能为数字字符，且个数必须是偶数，否则最后一个数据将被忽略而不被下发
	//命令无需专门回复，就像正常上传数据那样即可
	
	if(MODBUS_UnPacketCmd(&ls_addr, &m_cmd, &r_addr, &r_len, cmd, len) == 0)
	{
		UsartPrintf(USART_DEBUG, "从机地址: %X, 命令: %X, 寄存器地址: %X, 长度: %x\r\n",
									s_addr, m_cmd, r_addr, r_len);
		
		if(ls_addr == s_addr)
		{
			if(++rev_cmd_cnt >= 250)
				rev_cmd_cnt = 0;
		}
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MODBUS_UnPacketCmd Err\r\n");

}
