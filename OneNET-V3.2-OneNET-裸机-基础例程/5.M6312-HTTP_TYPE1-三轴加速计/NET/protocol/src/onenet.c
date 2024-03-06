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
#include "m6312.h"

//Э���ļ�
#include "onenet.h"

//Ӳ������
#include "usart.h"
#include "delay.h"
#include "adxl362.h"

//C��
#include <string.h>
#include <stdio.h>


#define DEVID	"5616708"

#define APIKEY	"kPl=kf99QxL2acVvVCHssUPWZKs="


void OneNet_FillBuf(char *buf)
{
	
	char text[64];
	char buf1[256];
	
	memset(text, 0, sizeof(text));
	memset(buf1, 0, sizeof(buf1));
	
	strcpy(buf1, "{\"datastreams\":[");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "{\"id\":\"Xg\",\"datapoints\":[{\"value\":%0.2f}]},", adxl362Info.x);
	strcat(buf1, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "{\"id\":\"Yg\",\"datapoints\":[{\"value\":%0.2f}]},", adxl362Info.y);
	strcat(buf1, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "{\"id\":\"Zg\",\"datapoints\":[{\"value\":%0.2f}]}", adxl362Info.z);
	strcat(buf1, text);
	
	strcat(buf1, "]}");
	
	sprintf(buf, "POST /devices/%s/datapoints?type=1 HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\n"
					"Content-Length:%d\r\n\r\n",
	
					DEVID, APIKEY, strlen(buf1));
					
	strcat(buf, buf1);

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
	
	char buf[512];
	
	memset(buf, 0, sizeof(buf));
	
	OneNet_FillBuf(buf);									//��װ������
	
	M6312_SendData((unsigned char *)buf, strlen(buf));		//�ϴ�����
	
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
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Tips:	Send Err\r\n");
		}
	}
	
	M6312_Clear();

}
