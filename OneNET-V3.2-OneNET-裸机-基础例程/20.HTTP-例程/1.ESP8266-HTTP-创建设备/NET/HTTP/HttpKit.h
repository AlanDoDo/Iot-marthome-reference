#ifndef _HTTPKIT_H_
#define _HTTPKIT_H_


#include "Common.h"


//=============================����==============================
//===========�����ṩRTOS���ڴ��������Ҳ����ʹ��C���=========
//RTOS
#include <stdlib.h>

#define HTTP_MallocBuffer	malloc

#define HTTP_FreeBuffer		free
//==========================================================


/*--------------------------------SAVEDATA��Ϣ֧�ֵĸ�ʽ����--------------------------------*/
typedef enum
{
	
    kTypeFullJson = 0x01,
	
    kTypeBin = 0x02,
	
    kTypeSimpleJsonWithoutTime = 0x03,
	
    kTypeSimpleJsonWithTime = 0x04,
	
    kTypeString = 0x05
	
} SaveDataType;


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
	
} HTTP_PACKET_STRUCTURE;


/*--------------------------------ɾ��--------------------------------*/
void HTTP_DeleteBuffer(HTTP_PACKET_STRUCTURE *httpPacket);

/*--------------------------------���ݵ��ϴ����--------------------------------*/
uint1 HTTP_Post_PacketSaveData(const int8 *devid, int8 *apikey, int16 send_len, int8 *type_bin_head, SaveDataType type, HTTP_PACKET_STRUCTURE *httpPacket);

uint1 HTTP_Post_PacketDeviceRegister(const int8 *apikey, const int8 *title, const int8 *auth_info, const int8 *desc,
										const int8 *Private, HTTP_PACKET_STRUCTURE *httpPacket);

uint1 HTTP_UnPacketDeviceRegister(uint8 *rev_data, int8 **devid);

#endif
