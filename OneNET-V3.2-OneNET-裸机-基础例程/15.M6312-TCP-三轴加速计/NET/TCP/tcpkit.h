#ifndef _TCPKIT_H_
#define _TCPKIT_H_


#include "Common.h"


//=============================����==============================
//===========�����ṩRTOS���ڴ��������Ҳ����ʹ��C���=========
#include <stdlib.h>

#define TCP_MallocBuffer	malloc

#define TCP_FreeBuffer		free
//==========================================================


#ifndef NULL
#define NULL (void*)0
#endif


/*--------------------------------��Ϣ����--------------------------------*/
#define TCP_CONNECT			0x10			//��¼�ɹ�����δ֪����

#define TCP_DATA			0x20			//������Ӧ

#define TCP_HEARTBEAT		0x30			//������Ӧ

#define TCP_DATA_CHECK		0x40			//�������ݲ�ѯ


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
	
} TCP_PACKET_STRUCTURE;


/*--------------------------------ɾ��--------------------------------*/
void TCP_DeleteBuffer(TCP_PACKET_STRUCTURE *tcpPacket);

/*--------------------------------���--------------------------------*/
uint8 TCP_UnPacketRecv(uint8 *dataPtr);

/*--------------------------------��¼��ʽ���--------------------------------*/
uint1 TCP_PacketConnect(const int8 *proid, const int8 *auth_info, const int8 *lua_name, TCP_PACKET_STRUCTURE *tcpPacket);

/*--------------------------------�ϴ��������--------------------------------*/
uint8 TCP_PacketData(int32 send_len, TCP_PACKET_STRUCTURE *tcpPacket);

/*--------------------------------�����������--------------------------------*/
uint1 TCP_PacketPing(TCP_PACKET_STRUCTURE *tcpPacket);


#endif
