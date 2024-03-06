/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	sample.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-06-14
	*
	*	版本： 		V1.0
	*
	*	说明： 		此文件不参与编译，仅仅只是SDK使用的演示
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/


#include "httpkit.h"

#include <string.h>


//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//				devid：设备ID
//				apikey：设备apikey
//				streamArray：数据流
//				streamArrayNum：数据流个数
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
_Bool OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	HTTP_PACKET_STRUCTURE httpPacket = {NULL, 0, 0, 0};
	
	_Bool status = 0;
	short body_len = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE%d\r\n", type);
	
//---------------------------------------------步骤一：测量数据流长度---------------------------------------------
	body_len = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt, 0);
	if(body_len)
	{
//---------------------------------------------步骤二：填写协议头-------------------------------------------------
		if(HTTP_Post_PacketSaveData(devid, apikey, body_len, NULL, (SaveDataType)type, &httpPacket) == 0)
		{
//---------------------------------------------步骤三：组包-------------------------------------------------------
			body_len = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, httpPacket._data, httpPacket._size, httpPacket._len);
			
			if(body_len)
			{
				httpPacket._len += body_len;
				UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", httpPacket._len);
//---------------------------------------------步骤四：发送数据---------------------------------------------------
				NET_DEVICE_SendData(httpPacket._data, httpPacket._len);
			}
			else
				UsartPrintf(USART_DEBUG, "WARN:	DSTREAM_GetDataStream_Body Failed\r\n");
			
//---------------------------------------------步骤五：删包-------------------------------------------------------
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
//	函数名称：	OneNet_RegisterDevice
//
//	函数功能：	注册设备
//
//	入口参数：	apikey：master-key(产品APIKEY)
//				title：设备名
//				auth_info:设备的鉴权信息
//				desc：描述信息
//				Private：是否公开
//
//	返回参数：	0-成功		1-失败
//
//	说明：		非HTTP协议的设备，调用此函数前需要断开当前连接，再连接HTTP协议的ip
//==========================================================
_Bool OneNet_RegisterDevice(const char *apikey, const char *title, const char *auth_info, const char *desc, const char *Private)
{
	
	HTTP_PACKET_STRUCTURE httpPacket = {NULL, 0, 0, 0};
	
//---------------------------------------------步骤一：组包---------------------------------------------
	if(HTTP_Post_PacketDeviceRegister(apikey, title, auth_info, desc, Private, &httpPacket) == 0)
	{
//---------------------------------------------步骤二：发送数据-----------------------------------------
		NET_DEVICE_SendData(httpPacket._data, httpPacket._len);
		
//---------------------------------------------步骤三：删包---------------------------------------------
		HTTP_DeleteBuffer(&httpPacket);
	}
	
	return 0;

}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
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
		//这里用来检测是否发送成功
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
