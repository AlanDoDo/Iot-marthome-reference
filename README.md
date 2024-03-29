# Iot-marthome-reference

## 物联网demo

![](https://alandodo-1315761622.cos.ap-beijing.myqcloud.com/blog/x302.jpg)

* 物料清单
1. STM32C8T6(进口)
2. ESP826601S
3. DHT11
4. 0.96OLEDIIC
5. 按键 4P 6*6*5 直插
6. 下载器：ST-LINK


## PCB设计

onenet-show工程

![](https://alandodo-1315761622.cos.ap-beijing.myqcloud.com/blog/x303.jpg)

## 初始化代码

1. 选择芯片 **STM32F103C8**
2. Define 高等容量 **STM32F10X_MD, USE_STDPERIPH_DRIVER**

![](https://alandodo-1315761622.cos.ap-beijing.myqcloud.com/blog/x297.jpg)

## 报错

> error: L6235E: More than one section matches selector - cannot all be FIRST/LAST.

原因：项目中同时包含以下启动文件，

    startup_stm32f10x_hd.s

    startup_stm32f10x_md.s

    startup_stm32f10x_ld.s
    ......

应该针对不同的CPU选择不同的启动文件。



1. 解决办法1：从项目中删除不相关的启动文件

2. 解决办法2：右击不相关的启动文件，点击 options for file 'startup_stm32f10x_md.s'...

在弹出的对话框中的properties页，勾去灰化的'Include in Target Build'和‘Always Build’两项。

* main.c
```c
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

```

## 点亮LED

```c
//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "led.h"


LED_INFO led_info = {0};


/*
************************************************************
*	函数名称：	Led_Init
*
*	函数功能：	LED初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/

void Led_Init(void)
{

	GPIO_InitTypeDef gpio_initstruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);		//打开GPIOB的时钟
	
	gpio_initstruct.GPIO_Mode = GPIO_Mode_Out_PP;				//设置为推挽输出模式
	gpio_initstruct.GPIO_Pin = GPIO_Pin_13;						//将初始化的Pin脚
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;				//可承载的最大频率
	
	GPIO_Init(GPIOC, &gpio_initstruct);							//初始化GPIO
	
	Led_Set(LED_OFF);											//初始化完成后，关闭LED

}

/*
************************************************************
*	函数名称：	Led_Set
*
*	函数功能：	LED控制
*
*	入口参数：	status：开关蜂鸣器
*
*	返回参数：	无
*
*	说明：		开-LED_ON		关-LED_OFF
************************************************************
*/

void Led_Set(_Bool status)
{
	
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, status == LED_ON ? Bit_RESET : Bit_SET);		//根据status参数设置LED引脚的电平状态
	
	led_info.Led_Status = status;    //更新全局变量led_info中的LED状态

}
```

## 按键控制LED

```c
//单片机头文件
#include "stm32f10x.h"

//硬件驱动
#include "key.h"
#include "delay.h"
#include "led.h"

/*
************************************************************
*	函数名称：	Key_Init
*
*	函数功能：	Key初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/

void Key_Init(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    EXTI_InitTypeDef exti_initstruct;
    NVIC_InitTypeDef nvic_initstruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);     //打开GPIOA的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    gpio_initstruct.GPIO_Mode = GPIO_Mode_IPU;                //设置为输入上拉模式
    gpio_initstruct.GPIO_Pin = GPIO_Pin_1;                    //将初始化的Pin脚设为GPIO_Pin_1，即PA1引脚
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;            //设置IO口的输入速度为50MHz
    GPIO_Init(GPIOA, &gpio_initstruct);                       //初始化GPIOA
    
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);    //将GPIOA的Pin1连接到外部中断线1
    
    exti_initstruct.EXTI_Line = EXTI_Line1;                   //选择外部中断线1
    exti_initstruct.EXTI_Mode = EXTI_Mode_Interrupt;          //设置为中断模式
    exti_initstruct.EXTI_Trigger = EXTI_Trigger_Falling;      //触发方式为下降沿
    exti_initstruct.EXTI_LineCmd = ENABLE;                    //使能外部中断线1
    EXTI_Init(&exti_initstruct);                              //初始化外部中断
    
    nvic_initstruct.NVIC_IRQChannel = EXTI1_IRQn;             //配置外部中断1的中断通道
    nvic_initstruct.NVIC_IRQChannelPreemptionPriority = 2;    //抢占优先级为2
    nvic_initstruct.NVIC_IRQChannelSubPriority = 2;           //子优先级为2
    nvic_initstruct.NVIC_IRQChannelCmd = ENABLE;              //使能外部中断1的中断
    NVIC_Init(&nvic_initstruct);                              //初始化外部中断中断向量表
}

void EXTI1_IRQHandler(void)
{
    DelayXms(10);   //延时10ms
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == 0)  //如果PA1引脚为低电平（按键按下）
    {
        if(led_info.Led_Status == LED_ON)
            Led_Set(LED_OFF);   //如果LED状态为开，则将LED关闭
        else
            Led_Set(LED_ON);    //如果LED状态为关，则将LED打开
    }
    EXTI_ClearITPendingBit(EXTI_Line1);  //清除外部中断线1的中断标志位
}
```

## 获取DHT11温湿度数据

```c
/***************STM32F103C8T6**********************
 * 文件名  ：DHT11.c
 * 描述    ：DHT11传感器
 * 备注    : DHT11温度湿度传感器
 * 接口    ：PA11-DATA

********************LIGEN*************************/

#include "dht11.h"
#include "delay.h"

#define DT GPIO_Pin_0
      
//复位DHT11
void DHT11_Rst(void)	   
{                 
	DHT11_IO_OUT(); 	//设置为输出模式
	DHT11_DQ_OUT(0); 	//拉低数据线
	DelayXms(20);    	//保持低电平至少18ms
	DHT11_DQ_OUT(1); 	//释放数据线
	DelayUs(30);     	//等待DHT11响应，拉高20~40us
}


//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
uint8_t DHT11_Check(void) 	   
{   
	uint8_t retry = 0;
	DHT11_IO_IN(); // 设置为输入模式	 
    while (DHT11_DQ_IN && retry < 100) // 等待 DHT11 拉低数据线，最多等待 100 次
	{
		retry++;
		DelayUs(1);
	};	 
	if (retry >= 100)
		return 1;
	else
		retry = 0;
    while (!DHT11_DQ_IN && retry < 100) // 等待 DHT11 释放数据线，最多等待 100 次
	{
		retry++;
		DelayUs(1);
	};
	if (retry >= 100)
		return 1;	    
	return 0;
}


//从DHT11读取一个位
//返回值：1/0
uint8_t DHT11_Read_Bit(void) 			 
{
 	uint8_t retry = 0;
	while (DHT11_DQ_IN && retry < 100) // 等待变为低电平，最多等待 100 次
	{
		retry++;
		DelayUs(1);
	}
	retry = 0;
	while (!DHT11_DQ_IN && retry < 100) // 等待变为高电平，最多等待 100 次
	{
		retry++;
		DelayUs(1);
	}
	DelayUs(40); // 等待 40us
	if (DHT11_DQ_IN)
		return 1;
	else
		return 0;		   
}


//从DHT11读取一个字节
//返回值：读到的数据
uint8_t DHT11_Read_Byte(void)    
{        
	uint8_t i,dat;
	dat=0;
	for (i=0;i<8;i++) 
	{
		dat<<=1; 
		dat|=DHT11_Read_Bit();
	}						    
	return dat;
}

//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi)    
{        
 	uint8_t buf[5];
	uint8_t i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
	}
	else return 1;
	return 0;	    
}

//初始化DHT11的IO口 DQ 同时检测DHT11的存在
//返回1:不存在
//返回0:存在    	 
uint8_t DHT11_Init(void)
{	 
 	GPIO_InitTypeDef  GPIO_InitStructure;	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PG端口时钟
 	GPIO_InitStructure.GPIO_Pin = DT;				 //PG11端口配置
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);				 //初始化IO口
 	GPIO_SetBits(GPIOA,DT);						 //PG11 输出高
			    
	DHT11_Rst();  //复位DHT11
	return DHT11_Check();//等待DHT11的回应
} 
```

## ESP8266连接WiFi

```c
#define ESP8266_WIFI_INFO		"AT+CWJAP=\"abc\",\"abc123456\"\r\n"
```

## 连接MQTT

```c
#define PROID			""

#define ACCESS_KEY		""

#define DEVICE_NAME		""

```

## App主要代码

```vue
<template>
	<view class="wrap">
		<view class="dev-area">

			<view class="dev-cart">
				<view class="">
					<view class="dev-name">温度</view>
					<image class="dev-logo" src="../../static/temp.png" mode=""></image>
				</view>
				<view class="dev-data">{{temp}} ℃</view>
			</view>
			<view class="dev-cart">
				<view class="">
					<view class="dev-name">湿度</view>
					<image class="dev-logo" src="../../static/humi.png" mode=""></image>
				</view>
				<view class="dev-data">{{humi}} ℃</view>
			</view>
			<view class="dev-cart">
				<view class="">
					<view class="dev-name">台灯</view>
					<image class="dev-logo" src="../../static/lamp.png" mode=""></image>
				</view>
				<switch :checked="led" @change="onLedSwitch" color="#2b9939" />
			</view>
			<view class="dev-cart">
				<view class="">
					扩展模块
				</view>
			</view>

		</view>
	</view>

</template>

<script>
	const {
		createCommonToken
	} = require('@/key.js')
	export default {
		data() {
			return {
				temp: '',
				humi: '',
				led: true,
				token: '',
			}
		},
		onLoad() {
			const params = {
				author_key: '',
				version: '2022-05-01',
				user_id: '',
			}
			this.token = createCommonToken(params);
		},
		onShow() {
			this.fetchDevData();
			setInterval(() => {
				this.fetchDevData();
			}, 3000)
		},
		methods: {
			fetchDevData() {
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/query-device-property', //仅为示例，并非真实接口地址。
					method: 'GET',
					data: {
						product_id: '695g29B4Zg',
						device_name: 'd1'
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: (res) => {
						console.log(res.data);
						this.temp = res.data.data[2].value;
						this.humi = res.data.data[0].value;
						this.led = res.data.data[1].value === 'true';

					}
				});
			},
			onLedSwitch(event) {
				console.log(event.detail.value);
				let value = event.detail.value;
				uni.request({
					url: 'https://iot-api.heclouds.com/thingmodel/set-device-property', //仅为示例，并非真实接口地址。
					method: 'POST',
					data: {
						product_id: '695g29B4Zg',
						device_name: 'd1',
						params: {
							"led": value
						}
					},
					header: {
						'authorization': this.token //自定义请求头信息
					},
					success: () => {
						console.log('LED ' + (value ? 'ON' : 'OFF') + ' !');
					}
				});
			}

		}
	}
</script>

<style>
	.wrap {
		padding: 30rpx;
	}

	.dev-area {
		display: flex;
		justify-content: space-between;
		flex-wrap: wrap;
	}

	.dev-cart {
		height: 150rpx;
		width: 320rpx;
		border-radius: 30rpx;
		margin-top: 30rpx;
		display: flex;
		justify-content: space-around;
		align-items: center;
		box-shadow: 0 0 15rpx #ccc;
	}

	.dev-name {
		font-size: 20rpx;
		text-align: center;
		color: #6d6d6d;
	}

	.dev-logo {
		width: 70rpx;
		height: 70rpx;
		margin-top: 10rpx;
	}

	.dev-data {
		font-size: 50rpx;
		color: #6d6d6d;
	}
</style>
```