/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	tcpkit.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2018-11-14
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		TCP͸��Э��
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//Э��ͷ�ļ�
#include "tcpkit.h"

//C��
#include <stdio.h>
#include <string.h>


//==========================================================
//	�������ƣ�	TCP_NewBuffer
//
//	�������ܣ�	�����ڴ�
//
//	��ڲ�����	tcpPacket�����ṹ��
//				size����С
//
//	���ز�����	��
//
//	˵����		1.��ʹ�ö�̬�����������ڴ�
//				2.��ʹ�þֲ���ȫ��������ָ���ڴ�
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
//	�������ƣ�	TCP_DeleteBuffer
//
//	�������ܣ�	�ͷ������ڴ�
//
//	��ڲ�����	tcpPacket�����ṹ��
//
//	���ز�����	��
//
//	˵����		��ʹ�õľֲ���ȫ������ʱ���ͷ��ڴ�
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
//	�������ƣ�	TCP_UnPacketRecv
//
//	�������ܣ�	TCP���ݽ��������ж�
//
//	��ڲ�����	dataPtr�����յ�����ָ��
//
//	���ز�����	0-�ɹ�		����-ʧ��ԭ��
//
//	˵����		
//==========================================================
uint8 TCP_UnPacketRecv(uint8 *dataPtr)
{
	
	return dataPtr[0];

}

//==========================================================
//	�������ƣ�	TCP_PacketConnect
//
//	�������ܣ�	��¼��ʽ���
//
//	��ڲ�����	proid����ƷID
//				auth_info����Ȩ��Ϣ
//				lua_name��ʹ�õĽű���
//				tcpPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
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
//	�������ƣ�	TCP_PacketData
//
//	�������ܣ�	��װЭ��ͷ
//
//	��ڲ�����	send_len���������ܳ�
//				edpPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
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
//	�������ƣ�	TCP_PacketPing
//
//	�������ܣ�	�����������
//
//	��ڲ�����	tcpPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
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
