/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	ModBusKit.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-12-14
	*
	*	版本： 		V1.0
	*
	*	说明： 		ModBus协议
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//协议头文件
#include "ModBusKit.h"

//C库
#include <string.h>


//==========================================================
//	函数名称：	MODBUS_NewBuffer
//
//	函数功能：	申请内存
//
//	入口参数：	modbusPacket：包结构体
//				size：大小
//
//	返回参数：	无
//
//	说明：		1.可使用动态分配来分配内存
//				2.可使用局部或全局数组来指定内存
//==========================================================
void MODBUS_NewBuffer(MODBUS_PACKET_STRUCTURE *modbusPacket, uint32 size)
{
	
	uint32 i = 0;

	if(modbusPacket->_data == NULL)
	{
		modbusPacket->_memFlag = MEM_FLAG_ALLOC;
		
		modbusPacket->_data = (uint8 *)MODBUS_MallocBuffer(size);
		if(modbusPacket->_data != NULL)
		{
			modbusPacket->_len = 0;
			
			modbusPacket->_size = size;

			for(; i < modbusPacket->_size; i++)
				modbusPacket->_data[i] = 0;
		}
	}
	else
	{
		modbusPacket->_memFlag = MEM_FLAG_STATIC;
		
		for(; i < modbusPacket->_size; i++)
			modbusPacket->_data[i] = 0;
		
		modbusPacket->_len = 0;
		
		if(modbusPacket->_size < size)
			modbusPacket->_data = NULL;
	}

}

//==========================================================
//	函数名称：	MODBUS_DeleteBuffer
//
//	函数功能：	释放数据内存
//
//	入口参数：	edpPacket：包结构体
//
//	返回参数：	无
//
//	说明：		当使用的局部或全局数组时不释放内存
//==========================================================
void MODBUS_DeleteBuffer(MODBUS_PACKET_STRUCTURE *modbusPacket)
{

	if(modbusPacket->_memFlag == MEM_FLAG_ALLOC)
		MODBUS_FreeBuffer(modbusPacket->_data);
	
	modbusPacket->_data = NULL;
	modbusPacket->_len = 0;
	modbusPacket->_size = 0;
	modbusPacket->_memFlag = MEM_FLAG_NULL;

}

//==========================================================
//	函数名称：	MODBUS_CRC16
//
//	函数功能：	CRC16校验
//
//	入口参数：	buf：待计算的buf
//				length：数据长度
//
//	返回参数：	CRC16校验结果
//
//	说明：		
//==========================================================
uint16 MODBUS_CRC16(uint8 *buf, uint16 length)
{
	
	uint16 i = 0, j = 0, tmp = 0;
	uint16 crc = 0xFFFF;
	
	for(i = 0; i < length; i++)
	{
		crc = buf[i] ^ crc;
		for(j = 0; j < 8; j++)
		{
			tmp = crc & 0x0001;
			crc = crc >> 1;
			if(tmp)
				crc = crc ^ 0xA001;
		}
	}
	
	return crc << 8 | crc >> 8;
	
}

//==========================================================
//	函数名称：	MODBUS_Connect
//
//	函数功能：	登录组包
//
//	入口参数：	serial：序列号
//				pswd：密码
//				proid：产品ID
//				modbusPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
uint1 MODBUS_Connect(const int8 *serial, const int8 *pswd, const int8 *proid, MODBUS_PACKET_STRUCTURE *modbusPacket)
{

	//分配内存---------------------------------------------------------------------
	MODBUS_NewBuffer(modbusPacket, 52);
	if(modbusPacket->_data == NULL)
		return 1;
	
	//Byte0~Byte10：报文:type,以0补齐11字节----------------------------------------
	strncpy((int8 *)modbusPacket->_data + modbusPacket->_len, "type", 4);
	modbusPacket->_len += 11;
	
	//Byte11~Byte19：报文:name,以0补齐9字节----------------------------------------
	strncpy((int8 *)modbusPacket->_data + modbusPacket->_len, "name", 4);
	modbusPacket->_len += 9;
	
	//Byte20~Byte31：报文:phone,以0补齐12字节-------------------------------------
	strncpy((int8 *)modbusPacket->_data + modbusPacket->_len, serial, strlen(serial));
	modbusPacket->_len += 12;
	
	//Byte32~Byte40：报文:svrpwd,以0补齐9字节-------------------------------------
	strncpy((int8 *)modbusPacket->_data + modbusPacket->_len, pswd, strlen(pswd));
	modbusPacket->_len += 9;
	
	//Byte41~Byte51：报文:id,以0补齐11字节-------------------------------------
	strncpy((int8 *)modbusPacket->_data + modbusPacket->_len, proid, strlen(proid));
	modbusPacket->_len += 11;
	
	return 0;

}

//==========================================================
//	函数名称：	MODBUS_PacketCmd
//
//	函数功能：	命令组包
//
//	入口参数：	s_addr：从机地址
//				m_cmd：modbus命令
//				value：传感器值缓存
//				value_cnt：传感器值个数
//				modbusPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
_Bool MODBUS_PacketCmd(uint8 s_addr, uint8 m_cmd, uint16 *value, uint8 value_cnt, MODBUS_PACKET_STRUCTURE *modbusPacket)
{
	
	uint8 i = 0;
	uint16 crc = 0;
	
	//分配内存---------------------------------------------------------------------
	MODBUS_NewBuffer(modbusPacket, 5 + (value_cnt << 1));
	if(modbusPacket->_data == NULL)
		return 1;
	
	//Byte0：从机地址--------------------------------------------------------------
	modbusPacket->_data[modbusPacket->_len++] = s_addr;
	
	//Byte1：命令码----------------------------------------------------------------
	modbusPacket->_data[modbusPacket->_len++] = m_cmd;
	
	//Byte2：数据长度(字节)--------------------------------------------------------
	modbusPacket->_data[modbusPacket->_len++] = value_cnt << 1;
	
	//Byte3~x：上传的数据----------------------------------------------------------
	for(; i < value_cnt; i++)
	{
		modbusPacket->_data[modbusPacket->_len++] = value[i] >> 8;
		modbusPacket->_data[modbusPacket->_len++] = value[i] & 0x00FF;
	}
	
	//Bytex~x+2：CRC16校验码------------------------------------------------------
	crc = MODBUS_CRC16(modbusPacket->_data, modbusPacket->_len);
	
	modbusPacket->_data[modbusPacket->_len++] = crc >> 8;
	modbusPacket->_data[modbusPacket->_len++] = crc & 0x00FF;

	return 0;

}

//==========================================================
//	函数名称：	MODBUS_UnPacketCmd
//
//	函数功能：	命令解包
//
//	入口参数：	s_addr：从机地址
//				m_cmd：modbus命令
//				r_addr：寄存器地址
//				addr_len：读取长度
//				data：原始数据
//				len：数据长度
//
//	返回参数：	0-成功		其他-失败
//
//	说明：		
//==========================================================
uint8 MODBUS_UnPacketCmd(uint8 *s_addr, uint8 *m_cmd, uint16 *r_addr, uint16 *addr_len, uint8 *data, uint16 len)
{
	
	uint16 crc = 0;
	
	if(len < 3)
		return 1;
	
	crc = MODBUS_CRC16(data, len - 2);					//CRC16-高8位在前
	
	if((data[len - 2] << 8 | data[len - 1]) == crc)		//检查检验和是否正确
	{
		//提取从机地址--------------------------------------------------------------
		*s_addr = data[0];
		
		//提取命令码----------------------------------------------------------------
		*m_cmd = data[1];
		
		//提取寄存器地址------------------------------------------------------------
		*r_addr = (uint16)data[2] << 8 | data[3];
		
		//提取读取长度--------------------------------------------------------------
		*addr_len = (uint16)data[4] << 8 | data[5];
		
		return 0;
	}
	else
		return 2;										//CRC错误

}

//==========================================================
//	函数名称：	MODBUS_PacketPing
//
//	函数功能：	心跳请求组包
//
//	入口参数：	modbusPacket：包指针
//
//	返回参数：	0-成功		1-失败
//
//	说明：		
//==========================================================
uint1 MODBUS_PacketPing(MODBUS_PACKET_STRUCTURE *modbusPacket)
{

	MODBUS_NewBuffer(modbusPacket, 2);
	if(modbusPacket->_data == NULL)
		return 1;
	
	//Byte0：0----------------------------------------------------------------------
	modbusPacket->_data[modbusPacket->_len++] = 0;
	
	//Byte1：0----------------------------------------------------------------------
	modbusPacket->_data[modbusPacket->_len++] = 0;
	
	return 0;

}
