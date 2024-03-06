/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
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
#include "m6312.h"

//Э���ļ�
#include "onenet.h"
#include "tcpkit.h"

//Ӳ������
#include "usart.h"
#include "delay.h"
#include "led.h"

//C��
#include <string.h>
#include <stdio.h>


#define PROID		"187421"

#define AUTH_INFO	"test"

#define LUA_NAME	"tcpled"


//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	TCP_PACKET_STRUCTURE tcpPacket = {NULL, 0, 0, 0};				//Э���

	unsigned char *dataPtr;
	
	unsigned char status = 1;
	
	UsartPrintf(USART_DEBUG, "OneNET_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	LUA:%s\r\n"
                        , PROID, AUTH_INFO, LUA_NAME);

	if(TCP_PacketConnect(PROID, AUTH_INFO, LUA_NAME, &tcpPacket) == 0)
	{
		M6312_SendData(tcpPacket._data, tcpPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = M6312_GetIPD(250);								//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)
		{
			if(TCP_UnPacketRecv(dataPtr) == TCP_CONNECT)
			{
				UsartPrintf(USART_DEBUG, "Tips:	���ӳɹ�\r\n");
				status = 0;
			}
		}
		
		TCP_DeleteBuffer(&tcpPacket);								//ɾ��
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	TCP_PacketConnect Failed\r\n");
	
	return status;
	
}

void OneNet_FillBuf(unsigned char *buf, unsigned char offset)
{
	
	unsigned char *data_ptr = buf + offset;
	
	*data_ptr++ = led_status.Led4Sta;
	*data_ptr++ = led_status.Led5Sta;
	*data_ptr++ = led_status.Led6Sta;
	*data_ptr++ = led_status.Led7Sta;

}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(void)
{
	
	TCP_PACKET_STRUCTURE tcpPacket = {NULL, 0, 0, 0};											//Э���
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-TCP\r\n");
	
	if(TCP_PacketData(4, &tcpPacket) == 0)
	{
		OneNet_FillBuf(tcpPacket._data, 3);
		tcpPacket._len = 9;
		
		M6312_SendData(tcpPacket._data, tcpPacket._len);										//�ϴ����ݵ�ƽ̨
		UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", tcpPacket._len);
		
		TCP_DeleteBuffer(&tcpPacket);															//ɾ��
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	
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
void OneNet_RevPro(unsigned char *cmd)
{

	char *dataPtr = NULL;
	char *req = (char *)cmd;
	char numBuf[10];
	int num = 0;
	
	dataPtr = strchr(req, ':');							//����':'
	
	if(dataPtr != NULL)									//����ҵ���
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
		
		if(strstr((char *)req, "redled"))				//����"redled"
		{
			if(num == 1)
			{
				Led4_Set(LED_ON);
			}
			else if(num == 0)
			{
				Led4_Set(LED_OFF);
			}
		}
														//��ͬ
		else if(strstr((char *)req, "greenled"))
		{
			if(num == 1)
			{
				Led5_Set(LED_ON);
			}
			else if(num == 0)
			{
				Led5_Set(LED_OFF);
			}
		}
		else if(strstr((char *)req, "yellowled"))
		{
			if(num == 1)
			{
				Led6_Set(LED_ON);
			}
			else if(num == 0)
			{
				Led6_Set(LED_OFF);
			}
		}
		else if(strstr((char *)req, "blueled"))
		{
			if(num == 1)
			{
				Led7_Set(LED_ON);
			}
			else if(num == 0)
			{
				Led7_Set(LED_OFF);
			}
		}
	}
	
	M6312_Clear();										//��ջ���

}
