/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	main.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.0
	*
	*	说明： 		接入onenet，上传数据和命令控制
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络协议层
#include "onenet.h"

//网络设备
#include "m6312.h"

//硬件驱动
#include "delay.h"
#include "beep.h"
#include "usart.h"
#include "adxl362.h"

//C库
#include <string.h>
#include <stdlib.h>


extern unsigned char data_bytes;

extern unsigned char m_cmd, rev_cmd_cnt;

extern unsigned char r_addr, r_len;


/*
************************************************************
*	函数名称：	Hardware_Init
*
*	函数功能：	硬件初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		初始化单片机功能以及外接设备
************************************************************
*/
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置

	Delay_Init();									//systick初始化
	
	Usart1_Init(115200);							//串口1，打印信息用
	
	Usart2_Init(115200);							//串口2，驱动ESP8266用
	
	ADXL362_Init();
	
	Beep_Init();									//蜂鸣器初始化
	
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	
}

/*
************************************************************
*	函数名称：	main
*
*	函数功能：	
*
*	入口参数：	无
*
*	返回参数：	0
*
*	说明：		
************************************************************
*/
int main(void)
{
	
	unsigned char *dataPtr = NULL;
	
	Hardware_Init();				//初始化外围硬件
	
	M6312_Init();					//初始化M6312
	
	while(OneNet_DevLink())			//接入OneNET
		DelayXms(500);
	
	Beep_Set(BEEP_ON);				//鸣叫提示接入成功
	DelayXms(250);
	Beep_Set(BEEP_OFF);
	
	while(1)
	{
		
		dataPtr = M6312_GetIPD(0);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr, data_bytes);
		
		if(rev_cmd_cnt > 0)											//如果收到平台的查询命令
		{
			--rev_cmd_cnt;
			
			//根据命令码、寄存器地址、寄存器地址长度吧要上传的值放入链表里
			//本例程命令码为3代表查询传感器数据，为4代表控制相关外设
			switch(m_cmd)
			{
				case 3:												//控制外设
				{
					unsigned short *value_table = malloc(r_len << 1);
					_Bool flag = 1;
					
					ADXL362_GetValue();
					
					if(value_table != NULL)
					{
						switch(r_addr)
						{
							//010300030003F5CB
							case 3:
							
								value_table[0] = (unsigned short )(adxl362Info.x * 100);
								value_table[1] = (unsigned short )(adxl362Info.y * 100);
								value_table[2] = (unsigned short )(adxl362Info.z * 100);
							
							break;
							
							default: flag = 0; break;
						}
						
						if(flag)
							OneNet_SendData(value_table, r_len);
						
						free(value_table);
					}
				}
				break;
				
				default:
				break;
			}
		}
		
		DelayXms(10);
	
	}

}
