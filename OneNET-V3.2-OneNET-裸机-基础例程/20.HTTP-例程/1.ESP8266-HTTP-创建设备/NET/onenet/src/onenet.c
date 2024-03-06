/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-02-27
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
	*
	*	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸
#include "esp8266.h"

//Э���ļ�
#include "onenet.h"
#include "httpkit.h"

//Ӳ������
#include "usart.h"
#include "delay.h"

//C��
#include <string.h>
#include <stdio.h>


#define DEVID	"5616708"

#define APIKEY	"kPl=kf99QxL2acVvVCHssUPWZKs="


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
//	���ز�����	��
//
//	˵����		��HTTPЭ����豸�����ô˺���ǰ��Ҫ�Ͽ���ǰ���ӣ�������HTTPЭ���ip
//==========================================================
void OneNet_RegisterDevice(const char *apikey, const char *title, const char *auth_info, const char *desc, const char *Private)
{
	
	HTTP_PACKET_STRUCTURE httpPacket = {NULL, 0, 0, 0};							//Э���
	
	if(HTTP_Post_PacketDeviceRegister(apikey, title, auth_info, desc, Private, &httpPacket) == 0)
	{
		ESP8266_SendData(httpPacket._data, httpPacket._len);				//�ϴ����ݵ�ƽ̨
		
		HTTP_DeleteBuffer(&httpPacket);										//ɾ��
	}

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
				char *devid = NULL;
				
				UsartPrintf(USART_DEBUG, "\r\n%s\r\n", dataPtr);
				if(HTTP_UnPacketDeviceRegister(dataPtr, &devid) == 0)
				{
					UsartPrintf(USART_DEBUG, "devid: %s\r\n", devid);
					HTTP_FreeBuffer(devid);
				}
			}
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Tips:	Send Err\r\n");
		}
	}
	
	ESP8266_Clear();

}
