/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	HttpKit.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-27
	*
	*	版本： 		V1.0
	*
	*	说明： 		HTTP协议
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//协议头文件
#include "httpkit.h"
#include "usart.h"
//C库
#include <stdio.h>
#include <string.h>


//==========================================================
//	函数名称：	HTTP_NewBuffer
//
//	函数功能：	申请内存
//
//	入口参数：	edpPacket：包结构体
//				size：大小
//
//	返回参数：	无
//
//	说明：		如果使用的
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
//	函数名称：	HTTP_DeleteBuffer
//
//	函数功能：	释放数据内存
//
//	入口参数：	edpPacket：包结构体
//
//	返回参数：	无
//
//	说明：		
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
//	函数名称：	HTTP_Post_PacketSaveData
//
//	函数功能：	上传数据组包
//
//	入口参数：	devid：设备ID
//				apikey:apikey
//				send_buf：json缓存buf
//				send_len：json总长
//				type_bin_head：bin文件的消息头
//				type：类型
//				httpPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
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
//	函数名称：	HTTP_Post_PacketDeviceRegister
//
//	函数功能：	注册设备组包
//
//	入口参数：	apikey：master-key(产品APIKEY)
//				title：设备名
//				auth_info:设备的鉴权信息
//				desc：描述信息
//				Private：是否公开
//				httpPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
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
//	函数名称：	HTTP_UnPacketDeviceRegister
//
//	函数功能：	注册设备解包
//
//	入口参数：	rev_data：接收到的数据
//				devid：注册好的设备ID
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
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
		
		while(*dataPtr < '0' || *dataPtr > '9')					//找到数据长度开头
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
