#ifndef _MODBUSKIT_H_
#define _MODBUSKIT_H_


#include "Common.h"


//=============================����==============================
//===========�����ṩRTOS���ڴ��������Ҳ����ʹ��C���=========
#include <stdlib.h>

#define MODBUS_MallocBuffer		malloc

#define MODBUS_FreeBuffer		free
//==========================================================


#ifndef NULL
#define NULL (void*)0
#endif


/*--------------------------------�ڴ���䷽����־--------------------------------*/
#define MEM_FLAG_NULL		0
#define MEM_FLAG_ALLOC		1
#define MEM_FLAG_STATIC		2


typedef struct Buffer
{
	
	uint8	*_data;		//Э������
	
	uint32	_len;		//д������ݳ���
	
	uint32	_size;		//�����ܴ�С
	
	uint8	_memFlag;	//�ڴ�ʹ�õķ�����0-δ����	1-ʹ�õĶ�̬����		2-ʹ�õĹ̶��ڴ�
	
} MODBUS_PACKET_STRUCTURE;


/*--------------------------------ɾ��--------------------------------*/
void MODBUS_DeleteBuffer(MODBUS_PACKET_STRUCTURE *edpPacket);

/*--------------------------------��¼���--------------------------------*/
uint1 MODBUS_Connect(const int8 *serial, const int8 *pswd, const int8 *devid, MODBUS_PACKET_STRUCTURE *modbusPacket);

/*--------------------------------�������--------------------------------*/
_Bool MODBUS_PacketCmd(uint8 s_addr, uint8 m_cmd, uint16 *value, uint8 value_cnt, MODBUS_PACKET_STRUCTURE *modbusPacket);

/*--------------------------------������--------------------------------*/
uint8 MODBUS_UnPacketCmd(uint8 *s_addr, uint8 *m_cmd, uint16 *r_addr, uint16 *addr_len, uint8 *data, uint16 len);

/*--------------------------------�����������--------------------------------*/
uint1 MODBUS_PacketPing(MODBUS_PACKET_STRUCTURE *modbusPacket);


#endif
