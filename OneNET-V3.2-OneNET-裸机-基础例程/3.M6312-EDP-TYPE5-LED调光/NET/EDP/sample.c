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


#include "edpkit.h"


unsigned char dataMem[64];		//全局数组方式


//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	devid：创建设备的devid或产品ID
//				auth_key：创建设备的masterKey或apiKey或设备鉴权信息
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_DevLink(const char* devid, const char* auth_key)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};
	
	edpPacket._data = dataMem;				//全局数组方式
	edpPacket._len = sizeof(dataMem);		//全局数组方式

//---------------------------------------------步骤一：组包---------------------------------------------
	if(EDP_PacketConnect1(devid, auth_key, 256, &edpPacket) == 0)
	{
//---------------------------------------------步骤二：发送连接数据--------------------------------------
		NET_DEVICE_SendData(edpPacket._data, edpPacket._len);
		
//---------------------------------------------步骤三：判断返回类型--------------------------------------
		if(EDP_UnPacketRecv(dataPtr) == CONNRESP)
		{
//---------------------------------------------步骤四：解析返回结果--------------------------------------
			switch(EDP_UnPacketConnectRsp(dataPtr))
			{
				case 0:
					UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");
					oneNetInfo.netWork = 1;
				break;
				
				case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
				case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：设备ID鉴权失败\r\n");break;
				case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
				case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户ID鉴权失败\r\n");break;
				case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：未授权\r\n");break;
				case 6:UsartPrintf(USART_DEBUG, "WARN:	连接失败：授权码无效\r\n");break;
				case 7:UsartPrintf(USART_DEBUG, "WARN:	连接失败：激活码未分配\r\n");break;
				case 8:UsartPrintf(USART_DEBUG, "WARN:	连接失败：该设备已被激活\r\n");break;
				case 9:UsartPrintf(USART_DEBUG, "WARN:	连接失败：重复发送连接请求包\r\n");break;
				
				default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
			}
		}
		
//---------------------------------------------步骤五：删包---------------------------------------------
		EDP_DeleteBuffer(&edpPacket);
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	EDP_PacketConnect Failed\r\n");
	
}

//==========================================================
//	函数名称：	OneNet_PushData
//
//	函数功能：	PUSHDATA
//
//	入口参数：	dst_devid：接收设备的devid
//				data：数据内容
//				data_len：数据长度
//
//	返回参数：	无
//
//	说明：		设备与设备之间的通信
//==========================================================
_Bool OneNet_PushData(const char* dst_devid, const char* data, unsigned int data_len)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};

//---------------------------------------------步骤一：组包---------------------------------------------
	if(EDP_PacketPushData(dst_devid, data, data_len, &edpPacket) == 0)
	{
//---------------------------------------------步骤二：发送数据-----------------------------------------
		NET_DEVICE_SendData(edpPacket._data, edpPacket._len);
		
//---------------------------------------------步骤三：删包---------------------------------------------
		EDP_DeleteBuffer(&edpPacket);
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	OneNet_PushData Failed\r\n");
	
	return 0;

}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
_Bool OneNet_SendData(FORMAT_TYPE type, char *devid, char *apikey, DATA_STREAM *streamArray, unsigned short streamArrayCnt)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};
	
	_Bool status = 0;
	short body_len = 0;
	
	if(type < 1 && type > 5)
		return 1;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-EDP_TYPE%d\r\n", type);
	
	if(type != kTypeBin)
	{
//---------------------------------------------步骤一：测量数据流长度---------------------------------------------
		body_len = DSTREAM_GetDataStream_Body_Measure(type, streamArray, streamArrayCnt, 0);
		
		if(body_len)
		{
//---------------------------------------------步骤二：填写协议头-------------------------------------------------
			if(EDP_PacketSaveData(devid, body_len, NULL, (SaveDataType)type, &edpPacket) == 0)
			{
//---------------------------------------------步骤三：组包-------------------------------------------------------
				body_len = DSTREAM_GetDataStream_Body(type, streamArray, streamArrayCnt, edpPacket._data, edpPacket._size, edpPacket._len);
				
				if(body_len)
				{
					edpPacket._len += body_len;
//---------------------------------------------步骤四：发送数据---------------------------------------------------
					NET_DEVICE_SendData(edpPacket._data, edpPacket._len);
					UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", edpPacket._len);
				}
				else
					UsartPrintf(USART_DEBUG, "WARN:	DSTREAM_GetDataStream_Body Failed\r\n");
				
//---------------------------------------------步骤五：删包-------------------------------------------------------
				EDP_DeleteBuffer(&edpPacket);
			}
			else
				UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
		}
		else
			status = 1;
	}
	else
	{
		OneNet_SendData_Picture(devid, Array, sizeof(Array));
	}
	
	return status;
	
}

//==========================================================
//	函数名称：	OneNet_SendData_EDPType2
//
//	函数功能：	上传二进制数据到平台
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		若是低速设备，数据量大时，建议使用网络设备的透传模式
//				上传图片是，强烈建议devid字段为空，否则平台会将图片数据下发到设备
//==========================================================
#define PKT_SIZE 1024
void OneNet_SendData_Picture(char *devid, const char* picture, unsigned int pic_len)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};

	char type_bin_head[] = "{\"ds_id\":\"pic\"}";
	unsigned char *pImage = (unsigned char *)picture;
	
//---------------------------------------------步骤一：封装协议头---------------------------------------------
	if(EDP_PacketSaveData(devid, pic_len, type_bin_head, kTypeBin, &edpPacket) == 0)
	{
		UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", edpPacket._len);
//---------------------------------------------步骤二：发送协议头---------------------------------------------
		NET_DEVICE_SendData(edpPacket._data, edpPacket._len);
		
//---------------------------------------------步骤三：删包--------------------------------------------------
		EDP_DeleteBuffer(&edpPacket);
		
		UsartPrintf(USART_DEBUG, "image len = %d\r\n", pic_len);
		
//---------------------------------------------步骤四：发送bin文件数据----------------------------------------
		while(pic_len > 0)
		{
			RTOS_TimeDly(20);
			
			if(pic_len >= PKT_SIZE)
			{
				NET_DEVICE_SendData(pImage, PKT_SIZE);
				
				pImage += PKT_SIZE;
				pic_len -= PKT_SIZE;
			}
			else
			{
				NET_DEVICE_SendData(pImage, (unsigned short)pic_len);
				pic_len = 0;
			}
		}
		
		UsartPrintf(USART_DEBUG, "image send ok\r\n");
	}
	else
		UsartPrintf(USART_DEBUG, "EDP_PacketSaveData Failed\r\n");

}

//==========================================================
//	函数名称：	OneNet_HeartBeat
//
//	函数功能：	心跳检测
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_HeartBeat(void)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};
	unsigned char dataMem1[8];				//局部数组方式

	unsigned char sCount = 3;
	
	edpPacket._data = dataMem;				//局部数组方式
	edpPacket._len = sizeof(dataMem);		//局部数组方式
	
//---------------------------------------------步骤一：组包---------------------------------------------
	if(EDP_PacketPing(&edpPacket))
		return;
	
	while(sCount--)
	{
//---------------------------------------------步骤二：发送数据-----------------------------------------
		NET_DEVICE_SendData(edpPacket._data, edpPacket._len);
		
//---------------------------------------------步骤三：解析返回数据-------------------------------------
		if(EDP_UnPacketRecv(dataPtr) == PINGRESP)
		{
			UsartPrintf(USART_DEBUG, "Tips:	HeartBeat OK\r\n");
			
			break;
		}
		else
		{
			UsartPrintf(USART_DEBUG, "Check Device\r\n");
		}
		
		RTOS_TimeDly(2);
	}
	
//---------------------------------------------步骤四：删包---------------------------------------------
	EDP_DeleteBuffer(&edpPacket);

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
void OneNet_RevPro(unsigned char *cmd)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};
	
	char *cmdid_devid;
	unsigned short cmdid_len = 0;
	char *req;
	unsigned int req_len = 0;
	
	short result = 0;
	
//---------------------------------------------步骤一：获取返回数据类型---------------------------------------------
	switch(EDP_UnPacketRecv(cmd))						//判断是pushdata还是命令下发
	{
//---------------------------------------------步骤二：调用函数解析-------------------------------------------------
		case PUSHDATA:									//解pushdata包
			
			result = EDP_UnPacketPushData(cmd, &cmdid_devid, &req, &req_len);
		
			if(result == 0)
				UsartPrintf(USART_DEBUG, "src_devid: %s, req: %s, req_len: %d\r\n", cmdid_devid, req, req_len);
			
		break;
		
		case CMDREQ:									//解命令包
			
			result = EDP_UnPacketCmd(cmd, &cmdid_devid, &cmdid_len, &req, &req_len);
			
			if(result == 0)								//解包成功，则进行命令回复的组包
			{
				EDP_PacketCmdResp(cmdid_devid, cmdid_len, req, req_len, &edpPacket);
				UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_devid, req, req_len);
			}
			
		break;
			
		case SAVEACK:
			
			if(cmd[3] == MSG_ID_HIGH && cmd[4] == MSG_ID_LOW)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Send %s\r\n", cmd[5] ? "Err" : "Ok");
			}
			else
				UsartPrintf(USART_DEBUG, "Tips:	Message ID Err\r\n");
			
		break;
	}
	
//---------------------------------------------步骤三：命令处理---------------------------------------------
	if(req)
	{
		
	}
	
//---------------------------------------------步骤四：释放内存---------------------------------------------
	if(cmd[0] == PUSHDATA && result == 0)				//如果是pushdata 且 解包成功
	{
		EDP_FreeBuffer(cmdid_devid);					//释放内存
		EDP_FreeBuffer(req);
	}
//---------------------------------------------步骤五：回复命令、释放内存------------------------------------
	else if(cmd[0] == CMDREQ && result == 0)			//如果是命令包 且 解包成功
	{
		EDP_FreeBuffer(cmdid_devid);					//释放内存
		EDP_FreeBuffer(req);
														//回复命令
		NET_DEVICE_SendData(edpPacket._data, edpPacket._len);//上传平台
		EDP_DeleteBuffer(&edpPacket);					//删包
	}

}
