#ifndef _MODBUSKIT_H_
#define _MODBUSKIT_H_


#include "Common.h"


//=============================配置==============================
//===========可以提供RTOS的内存管理方案，也可以使用C库的=========
#include <stdlib.h>

#define MODBUS_MallocBuffer		malloc

#define MODBUS_FreeBuffer		free
//==========================================================


#ifndef NULL
#define NULL (void*)0
#endif


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
	
} MODBUS_PACKET_STRUCTURE;


/*--------------------------------删包--------------------------------*/
void MODBUS_DeleteBuffer(MODBUS_PACKET_STRUCTURE *edpPacket);

/*--------------------------------登录组包--------------------------------*/
uint1 MODBUS_Connect(const int8 *serial, const int8 *pswd, const int8 *devid, MODBUS_PACKET_STRUCTURE *modbusPacket);

/*--------------------------------命令组包--------------------------------*/
_Bool MODBUS_PacketCmd(uint8 s_addr, uint8 m_cmd, uint16 *value, uint8 value_cnt, MODBUS_PACKET_STRUCTURE *modbusPacket);

/*--------------------------------命令解包--------------------------------*/
uint8 MODBUS_UnPacketCmd(uint8 *s_addr, uint8 *m_cmd, uint16 *r_addr, uint16 *addr_len, uint8 *data, uint16 len);

/*--------------------------------心跳请求组包--------------------------------*/
uint1 MODBUS_PacketPing(MODBUS_PACKET_STRUCTURE *modbusPacket);


#endif
