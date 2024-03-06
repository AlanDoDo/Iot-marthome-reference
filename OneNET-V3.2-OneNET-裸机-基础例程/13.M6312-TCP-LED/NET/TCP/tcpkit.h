#ifndef _TCPKIT_H_
#define _TCPKIT_H_


#include "Common.h"


//=============================配置==============================
//===========可以提供RTOS的内存管理方案，也可以使用C库的=========
#include <stdlib.h>

#define TCP_MallocBuffer	malloc

#define TCP_FreeBuffer		free
//==========================================================


#ifndef NULL
#define NULL (void*)0
#endif


/*--------------------------------消息类型--------------------------------*/
#define TCP_CONNECT			0x10			//登录成功或者未知数据

#define TCP_DATA			0x20			//数据响应

#define TCP_HEARTBEAT		0x30			//心跳响应

#define TCP_DATA_CHECK		0x40			//主动数据查询


/*--------------------------------内存分配方案标志--------------------------------*/
#define MEM_FLAG_NULL		0
#define MEM_FLAG_ALLOC		1
#define MEM_FLAG_STATIC		2


typedef struct Buffer
{
	
	uint8	*_data;		//协议数据
	
	uint32	_len;		//写入的数据长度
	
	uint32	_size;		//缓存总大小
	
	uint8	_memFlag;	//内存使用的方案：0-未分配	1-使用的动态分配		2-使用的固定内存
	
} TCP_PACKET_STRUCTURE;


/*--------------------------------删包--------------------------------*/
void TCP_DeleteBuffer(TCP_PACKET_STRUCTURE *tcpPacket);

/*--------------------------------解包--------------------------------*/
uint8 TCP_UnPacketRecv(uint8 *dataPtr);

/*--------------------------------登录方式组包--------------------------------*/
uint1 TCP_PacketConnect(const int8 *proid, const int8 *auth_info, const int8 *lua_name, TCP_PACKET_STRUCTURE *tcpPacket);

/*--------------------------------上传数据组包--------------------------------*/
uint8 TCP_PacketData(int32 send_len, TCP_PACKET_STRUCTURE *tcpPacket);

/*--------------------------------心跳请求组包--------------------------------*/
uint1 TCP_PacketPing(TCP_PACKET_STRUCTURE *tcpPacket);


#endif
