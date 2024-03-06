#ifndef _HTTPKIT_H_
#define _HTTPKIT_H_


#include "Common.h"


//=============================配置==============================
//===========可以提供RTOS的内存管理方案，也可以使用C库的=========
//RTOS
#include <stdlib.h>

#define HTTP_MallocBuffer	malloc

#define HTTP_FreeBuffer		free
//==========================================================


/*--------------------------------SAVEDATA消息支持的格式类型--------------------------------*/
typedef enum
{
	
    kTypeFullJson = 0x01,
	
    kTypeBin = 0x02,
	
    kTypeSimpleJsonWithoutTime = 0x03,
	
    kTypeSimpleJsonWithTime = 0x04,
	
    kTypeString = 0x05
	
} SaveDataType;


/*--------------------------------内存分配方案标志--------------------------------*/
#define MEM_FLAG_NULL		0
#define MEM_FLAG_ALLOC		1
#define MEM_FLAG_STATIC		2


typedef struct Buffer
{
	
	uint8	*_data;		//协议数据
	
	uint32	_len;		//写入的数据长度
	
	uint32	_size;		//缓存总大小
	
	uint8	_memFlag;	//内存使用的方案：0-未分配	1-使用的动态分配		2-使用的固定内存
	
} HTTP_PACKET_STRUCTURE;


/*--------------------------------删包--------------------------------*/
void HTTP_DeleteBuffer(HTTP_PACKET_STRUCTURE *httpPacket);

/*--------------------------------数据点上传组包--------------------------------*/
uint1 HTTP_Post_PacketSaveData(const int8 *devid, int8 *apikey, int16 send_len, int8 *type_bin_head, SaveDataType type, HTTP_PACKET_STRUCTURE *httpPacket);

uint1 HTTP_Post_PacketDeviceRegister(const int8 *apikey, const int8 *title, const int8 *auth_info, const int8 *desc,
										const int8 *Private, HTTP_PACKET_STRUCTURE *httpPacket);

uint1 HTTP_UnPacketDeviceRegister(uint8 *rev_data, int8 **devid);

#endif
