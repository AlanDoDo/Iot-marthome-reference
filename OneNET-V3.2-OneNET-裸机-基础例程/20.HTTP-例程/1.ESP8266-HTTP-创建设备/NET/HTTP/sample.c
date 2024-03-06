/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	sample.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-06-14
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		���ļ���������룬����ֻ��SDKʹ�õ���ʾ
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/


#include "httpkit.h"

#include <string.h>


//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//				devid���豸ID
//				apikey���豸apikey
//				streamArray��������
//				streamArrayNum������������
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		
//==========================================================
_Bool OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	HTTP_PACKET_STRUCTURE httpPacket = {NULL, 0, 0, 0};
	
	_Bool status = 0;
	short body_len = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE%d\r\n", type);
	
//---------------------------------------------����һ����������������---------------------------------------------
	body_len = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt, 0);
	if(body_len)
	{
//---------------------------------------------���������дЭ��ͷ-------------------------------------------------
		if(HTTP_Post_PacketSaveData(devid, apikey, body_len, NULL, (SaveDataType)type, &httpPacket) == 0)
		{
//---------------------------------------------�����������-------------------------------------------------------
			body_len = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, httpPacket._data, httpPacket._size, httpPacket._len);
			
			if(body_len)
			{
				httpPacket._len += body_len;
				UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", httpPacket._len);
//---------------------------------------------�����ģ���������---------------------------------------------------
				NET_DEVICE_SendData(httpPacket._data, httpPacket._len);
			}
			else
				UsartPrintf(USART_DEBUG, "WARN:	DSTREAM_GetDataStream_Body Failed\r\n");
			
//---------------------------------------------�����壺ɾ��-------------------------------------------------------
			HTTP_DeleteBuffer(&httpPacket);
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	HTTP_NewBuffer Failed\r\n");
	}
	else
		status = 1;
	
	return status;
	
}

//==========================================================
//	�������ƣ�	OneNet_RegisterDevice
//
//	�������ܣ�	ע���豸
//
//	��ڲ�����	apikey��master-key(��ƷAPIKEY)
//				title���豸��
//				auth_info:�豸�ļ�Ȩ��Ϣ
//				desc��������Ϣ
//				Private���Ƿ񹫿�
//
//	���ز�����	0-�ɹ�		1-ʧ��
//
//	˵����		��HTTPЭ����豸�����ô˺���ǰ��Ҫ�Ͽ���ǰ���ӣ�������HTTPЭ���ip
//==========================================================
_Bool OneNet_RegisterDevice(const char *apikey, const char *title, const char *auth_info, const char *desc, const char *Private)
{
	
	HTTP_PACKET_STRUCTURE httpPacket = {NULL, 0, 0, 0};
	
//---------------------------------------------����һ�����---------------------------------------------
	if(HTTP_Post_PacketDeviceRegister(apikey, title, auth_info, desc, Private, &httpPacket) == 0)
	{
//---------------------------------------------���������������-----------------------------------------
		NET_DEVICE_SendData(httpPacket._data, httpPacket._len);
		
//---------------------------------------------��������ɾ��---------------------------------------------
		HTTP_DeleteBuffer(&httpPacket);
	}
	
	return 0;

}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *dataPtr)
{
	
	char *devid = NULL;

	if(strstr((char *)dataPtr, "CLOSED"))
	{
		UsartPrintf(USART_DEBUG, "TCP CLOSED\r\n");
	}
	else
	{
		//������������Ƿ��ͳɹ�
		if(strstr((char *)dataPtr, "succ"))
		{
			UsartPrintf(USART_DEBUG, "Tips:	Send OK\r\n");
			
			if(strstr((char *)dataPtr, "device_id"))
			{
				if(HTTP_UnPacketDeviceRegister(dataPtr, &devid) == 0)
				{
					UsartPrintf(USART_DEBUG, "devid: %s\r\n", devid);
					HTTP_FreeBuffer(devid);
				}
			}
			
			oneNetInfo.errCount = 0;
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Tips:	Send Err\r\n");
		}
	}

}
