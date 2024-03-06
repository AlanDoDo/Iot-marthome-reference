
//单片机头文件
#include "stm32f10x.h"

//网络协议层
#include "onenet.h"

//网络设备
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "dht11.h"
#include "oled.h"

//C库
#include <string.h>

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"

void Hardware_Init(void);
void Display_Init(void);
void Refresh_Data(void);

u8 temp,humi;

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
	
	unsigned short timeCount = 0;	//发送间隔变量
	
	unsigned char *dataPtr = NULL;
	
	Hardware_Init();				//初始化外围硬件
	
	ESP8266_Init();					//初始化ESP8266


//	UsartPrintf(USART_DEBUG, "Connect MQTTs Server...\r\n");
	OLED_Clear(); OLED_ShowString(0,0,"Connect MQTTs Server...",16);
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		DelayXms(500);
//	UsartPrintf(USART_DEBUG, "Connect MQTT Server Success\r\n");
	OLED_ShowString(0,4,"Connect MQTT Server Success",16); DelayXms(500);

	OLED_Clear(); OLED_ShowString(0,0,"Device login ...",16);
	while(OneNet_DevLink())			//接入OneNET
	{
		ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT");
		DelayXms(500);
	}
		

	OneNET_Subscribe();
	
	Display_Init();
	while(1)
	{

		if(++timeCount >= 100)									//发送间隔5s
		{
			DHT11_Read_Data(&temp,&humi);
			
//			UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");
			OneNet_SendData();									//发送数据
			
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(0);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		
		Refresh_Data();
		
		DelayMs(10);
	
	}

}
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
	
  Key_Init();
	
	Led_Init();									//蜂鸣器初始化
	
	OLED_Init();			//初始化OLED  
	
	while(DHT11_Init())
	{
//		UsartPrintf(USART_DEBUG, "DHT11 Error \r\n");
		OLED_ShowString(0,0,"DHT11 Error",16);
		DelayMs(1000);
	}
	
//	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
	OLED_Clear(); OLED_ShowString(0,0,"Hardware init OK",16); DelayMs(1000);
}

void Display_Init(void)
{
	
	OLED_Clear();
	
	OLED_ShowCHinese(0,0,1);//温
	OLED_ShowCHinese(18,0,2);//度
	OLED_ShowCHinese(36,0,0);//：
	OLED_ShowCHinese(72,0,3);//℃
	
	OLED_ShowCHinese(0,3,4);//湿
	OLED_ShowCHinese(18,3,5);//度
	OLED_ShowCHinese(36,3,0);//：
	OLED_ShowString(72,3,"%",16);//%
	
	OLED_ShowCHinese(0,6,6);//台
	OLED_ShowCHinese(18,6,7);//灯
	OLED_ShowCHinese(36,6,0);//：
	
}
void Refresh_Data(void)
{
	char buf[3];
	sprintf(buf, "%2d", temp);
	OLED_ShowString(54,0,buf,16); //温度值
	
	sprintf(buf, "%2d", humi);
	OLED_ShowString(54,3,buf,16); //湿度值
	
	if(led_info.Led_Status) OLED_ShowCHinese(54,6,8);//亮
	else OLED_ShowCHinese(54,6,9);//灭
	
}
