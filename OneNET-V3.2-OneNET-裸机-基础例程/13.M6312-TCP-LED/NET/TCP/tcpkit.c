/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	tcpkit.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2018-11-14
	*
	*	版本： 		V1.0
	*
	*	说明： 		TCP透传协议
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//协议头文件
#include "tcpkit.h"

//C库
#include <stdio.h>
#include <string.h>


//==========================================================
//	函数名称：	TCP_NewBuffer
//
//	函数功能：	申请内存
//
//	入口参数：	tcpPacket：包结构体
//				size：大小
//
//	返回参数：	无
//
//	说明：		1.可使用动态分配来分配内存
//				2.可使用局部或全局数组来指定内存
//==========================================================
void TCP_NewBuffer(TCP_PACKET_STRUCTURE *tcpPacket, uint32 size)
{
	
	uint32 i = 0;

	if(tcpPacket->_data == NULL)
	{
		tcpPacket->_memFlag = MEM_FLAG_ALLOC;
		
		tcpPacket->_data = (uint8 *)TCP_MallocBuffer(size);
		if(tcpPacket->_data != NULL)
		{
			tcpPacket->_len = 0;
			
			tcpPacket->_size = size;

			for(; i < tcpPacket->_size; i++)
				tcpPacket->_data[i] = 0;
		}
	}
	else
	{
		tcpPacket->_memFlag = MEM_FLAG_STATIC;
		
		for(; i < tcpPacket->_size; i++)
			tcpPacket->_data[i] = 0;
		
		tcpPacket->_len = 0;
		
		if(tcpPacket->_size < size)
			tcpPacket->_data = NULL;
	}

}

//==========================================================
//	函数名称：	TCP_DeleteBuffer
//
//	函数功能：	释放数据内存
//
//	入口参数：	tcpPacket：包结构体
//
//	返回参数：	无
//
//	说明：		当使用的局部或全局数组时不释放内存
//==========================================================
void TCP_DeleteBuffer(TCP_PACKET_STRUCTURE *tcpPacket)
{

	if(tcpPacket->_memFlag == MEM_FLAG_ALLOC)
		TCP_FreeBuffer(tcpPacket->_data);
	
	tcpPacket->_data = NULL;
	tcpPacket->_len = 0;
	tcpPacket->_size = 0;
	tcpPacket->_memFlag = MEM_FLAG_NULL;

}

//==========================================================
//	函数名称：	TCP_UnPacketRecv
//
//	函数功能：	TCP数据接收类型判断
//
//	入口参数：	dataPtr：接收的数据指针
//
//	返回参数：	0-成功		其他-失败原因
//
//	说明：		
//==========================================================
uint8 TCP_UnPacketRecv(uint8 *dataPtr)
{
	
	return dataPtr[0];

}

//==========================================================
//	函数名称：	TCP_PacketConnect
//
//	函数功能：	登录方式组包
//
//	入口参数：	proid：产品ID
//				auth_info：鉴权信息
//				lua_name：使用的脚本名
//				tcpPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
uint1 TCP_PacketConnect(const int8 *proid, const int8 *auth_info, const int8 *lua_name, TCP_PACKET_STRUCTURE *tcpPacket)
{

	if(proid == NULL || auth_info == NULL || lua_name == NULL)
		return 1;
	
	TCP_NewBuffer(tcpPacket, strlen(proid) + strlen(auth_info) + strlen(lua_name) + 5);
	if(tcpPacket->_data == NULL)
		return 1;
	
	snprintf((int8 *)tcpPacket->_data, tcpPacket->_size, "*%s#%s#%s*", proid, auth_info, lua_name);
	
	tcpPacket->_len = strlen((int8 *)tcpPacket->_data);
	
	return 0;

}

//==========================================================
//	函数名称：	TCP_PacketData
//
//	函数功能：	封装协议头
//
//	入口参数：	send_len：数据体总长
//				edpPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
uint8 TCP_PacketData(int32 send_len, TCP_PACKET_STRUCTURE *tcpPacket)
{

	if(!send_len)
		return 1;
	
	TCP_NewBuffer(tcpPacket, send_len + 5);
	if(tcpPacket->_data == NULL)
		return 2;
	
	tcpPacket->_data[0] = 0xAA;
	tcpPacket->_data[1] = 0x55;
	tcpPacket->_data[2] = send_len;
	
	tcpPacket->_data[send_len + 3] = 0x55;
	tcpPacket->_data[send_len + 4] = 0xAA;
	
	return 0;

}

//==========================================================
//	函数名称：	TCP_PacketPing
//
//	函数功能：	心跳请求组包
//
//	入口参数：	tcpPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
uint1 TCP_PacketPing(TCP_PACKET_STRUCTURE *tcpPacket)
{

	TCP_NewBuffer(tcpPacket, 2);
	if(tcpPacket->_data == NULL)
		return 1;
	
	tcpPacket->_data[tcpPacket->_len++] = 0x55;
	
	tcpPacket->_data[tcpPacket->_len++] = 0xAA;
	
	return 0;

}
