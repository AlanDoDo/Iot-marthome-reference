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
#include "esp8266.h"

//Э���ļ�
#include "onenet.h"
#include "modbuskit.h"

//Ӳ������
#include "usart.h"
#include "delay.h"

//C��
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#define PROID	"113237"

#define SERIAL	"1234567"

#define PSWD	"1234567"


#define MODBUS_SLAVE_ADDR		1


unsigned char s_addr = 0, m_cmd = 0, rev_cmd_cnt = 0;
unsigned short r_addr = 0, r_len = 0;


extern unsigned char esp8266_buf[128];


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
	
	MODBUS_PACKET_STRUCTURE modbusPacket = {NULL, 0, 0, 0};				//Э���
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\nPROID: %s,	SERIAL: %s,	PSWD: %s\r\n"
								, PROID, SERIAL, PSWD);
	
	if(MODBUS_Connect(SERIAL, PSWD, PROID, &modbusPacket) == 0)
	{
		ESP8266_SendData(modbusPacket._data, modbusPacket._len);		//�ϴ�ƽ̨
		
		MODBUS_DeleteBuffer(&modbusPacket);								//ɾ��
		
		s_addr = MODBUS_SLAVE_ADDR;
		
		return 0;
	}
	else
		return 1;
	
}

//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	buf����Ҫ���͵�����
//				dataLen�����ݳ���(���ָ���)
//
//	���ز�����	0-�ɹ�	����-ʧ��
//
//	˵����		
//==========================================================
unsigned char OneNet_SendData(unsigned short *buf ,unsigned short dataLen)
{
	
	MODBUS_PACKET_STRUCTURE modbusPacket = {NULL, 0, 0, 0};				//Э���
	
	if(MODBUS_PacketCmd(s_addr, m_cmd, buf, dataLen, &modbusPacket) == 0)
	{
		UsartPrintf(USART_DEBUG, "OneNet_SendData %d Bytes\r\n", modbusPacket._len);
		ESP8266_SendData(modbusPacket._data, modbusPacket._len);
	}
	
	return 1;

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
void OneNet_RevPro(unsigned char *cmd, unsigned short len)
{
	
	unsigned char ls_addr = 0;
	
	//ƽ̨�ֶ������·���ʽ��010100010001AC0A
	//ע��������ֻ��Ϊ�����ַ����Ҹ���������ż�����������һ�����ݽ������Զ������·�
	//��������ר�Żظ������������ϴ�������������
	
	if(MODBUS_UnPacketCmd(&ls_addr, &m_cmd, &r_addr, &r_len, cmd, len) == 0)
	{
		UsartPrintf(USART_DEBUG, "�ӻ���ַ: %X, ����: %X, �Ĵ�����ַ: %X, ����: %x\r\n",
									s_addr, m_cmd, r_addr, r_len);
		
		if(ls_addr == s_addr)
		{
			if(++rev_cmd_cnt >= 250)
				rev_cmd_cnt = 0;
		}
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MODBUS_UnPacketCmd Err\r\n");

}
