/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	HttpKit.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-27
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		HTTPЭ��
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//Э��ͷ�ļ�
#include "httpkit.h"
#include "usart.h"
//C��
#include <stdio.h>
#include <string.h>


//==========================================================
//	�������ƣ�	HTTP_NewBuffer
//
//	�������ܣ�	�����ڴ�
//
//	��ڲ�����	edpPacket�����ṹ��
//				size����С
//
//	���ز�����	��
//
//	˵����		���ʹ�õ�
//==========================================================
void HTTP_NewBuffer(HTTP_PACKET_STRUCTURE *httpPacket, uint32 size)
{
	
	uint32 i = 0;

	if(httpPacket->_data == NULL)
	{
		httpPacket->_memFlag = MEM_FLAG_ALLOC;
		
		httpPacket->_data = (uint8 *)HTTP_MallocBuffer(size);
		if(httpPacket->_data != NULL)
		{
			httpPacket->_len = 0;
			
			httpPacket->_size = size;
			
			for(; i < httpPacket->_size; i++)
				httpPacket->_data[i] = 0;
		}
	}
	else
	{
		httpPacket->_memFlag = MEM_FLAG_STATIC;
		
		for(; i < httpPacket->_size; i++)
			httpPacket->_data[i] = 0;
		
		httpPacket->_len = 0;
		
		if(httpPacket->_size < size)
			httpPacket->_data = NULL;
	}

}

//==========================================================
//	�������ƣ�	HTTP_DeleteBuffer
//
//	�������ܣ�	�ͷ������ڴ�
//
//	��ڲ�����	edpPacket�����ṹ��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void HTTP_DeleteBuffer(HTTP_PACKET_STRUCTURE *httpPacket)
{

	if(httpPacket->_memFlag == MEM_FLAG_ALLOC)
		HTTP_FreeBuffer(httpPacket->_data);
	
	httpPacket->_data = NULL;
	httpPacket->_len = 0;
	httpPacket->_size = 0;
	httpPacket->_memFlag = MEM_FLAG_NULL;

}

//==========================================================
//	�������ƣ�	HTTP_Post_PacketSaveData
//
//	�������ܣ�	�ϴ��������
//
//	��ڲ�����	devid���豸ID
//				apikey:apikey
//				send_buf��json����buf
//				send_len��json�ܳ�
//				type_bin_head��bin�ļ�����Ϣͷ
//				type������
//				httpPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint1 HTTP_Post_PacketSaveData(const int8 *devid, int8 *apikey, int16 send_len, int8 *type_bin_head, SaveDataType type, HTTP_PACKET_STRUCTURE *httpPacket)
{
	
	HTTP_NewBuffer(httpPacket, send_len + 140);
	if(httpPacket->_data == NULL)
		return 1;

	snprintf((int8 *)httpPacket->_data, 140, "POST /devices/%s/datapoints?type=%d HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\n"
					"Content-Length:%d\r\n\r\n",
	
					devid, type, apikey, send_len);
	
	httpPacket->_len += strlen((int8 *)httpPacket->_data);
	
	return 0;

}

//==========================================================
//	�������ƣ�	HTTP_Post_PacketDeviceRegister
//
//	�������ܣ�	ע���豸���
//
//	��ڲ�����	apikey��master-key(��ƷAPIKEY)
//				title���豸��
//				auth_info:�豸�ļ�Ȩ��Ϣ
//				desc��������Ϣ
//				Private���Ƿ񹫿�
//				httpPacket����ָ��
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint1 HTTP_Post_PacketDeviceRegister(const int8 *apikey, const int8 *title, const int8 *auth_info, const int8 *desc,
										const int8 *Private, HTTP_PACKET_STRUCTURE *httpPacket)
{
	
	uint16 send_len = 48 + strlen(title) + strlen(auth_info) + strlen(desc) + strlen(Private);

	HTTP_NewBuffer(httpPacket, send_len + 140);
	if(httpPacket->_data == NULL)
		return 1;

	snprintf((int8 *)httpPacket->_data, 140 + send_len, "POST /devices HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\n"
					"Content-Length:%d\r\n\r\n"
					"{\"title\":\"%s\",\"desc\":\"%s\",\"auth_info\":\"%s\",\"private\":%s}",
	
					apikey, send_len,
					title, desc, auth_info, Private);
	
	httpPacket->_len += strlen((int8 *)httpPacket->_data);
	
	return 0;

}

//==========================================================
//	�������ƣ�	HTTP_UnPacketDeviceRegister
//
//	�������ܣ�	ע���豸���
//
//	��ڲ�����	rev_data�����յ�������
//				devid��ע��õ��豸ID
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
uint1 HTTP_UnPacketDeviceRegister(uint8 *rev_data, int8 **devid)
{
	
	uint8 devid_len = 0;
	int8 *dataPtr = strstr((int8 *)rev_data, "device_id");
	
	if(dataPtr != NULL)
	{
		*devid = HTTP_MallocBuffer(10);
		if(*devid == NULL)
			return 1;
		
		memset(*devid, 0, 10);
		
		while(*dataPtr < '0' || *dataPtr > '9')					//�ҵ����ݳ��ȿ�ͷ
			dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')
		{
			dataPtr++;
			devid_len++;
		}
		
		dataPtr -= devid_len;
		memcpy(*devid, dataPtr, devid_len);
		
		return 0;
	}
	else
		return 1;

}
