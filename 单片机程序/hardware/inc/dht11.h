#ifndef DHT11_H
#define DHT11_H

#include "stm32f10x.h"


//PA11
#define DHT11_IO_IN()  {GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=8;}  //设置 GPIOA 的模式为输入模式
#define DHT11_IO_OUT() {GPIOA->CRL&=0XFFFFFFF0;GPIOA->CRL|=3;}  //设置 GPIOA 的模式为输出模式 

//IO操作函数   
#define	DHT11_DQ_OUT(X)  GPIO_WriteBit(GPIOA, GPIO_Pin_0, X)    //设置数据线的输出状态
#define	DHT11_DQ_IN  GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)   //读取数据线的输入状态

uint8_t DHT11_Init(void);   //初始化DHT11
uint8_t DHT11_Read_Data(uint8_t *temp,uint8_t *humi);   //读取数据
uint8_t DHT11_Read_Byte(void);  //读取一个字节
uint8_t DHT11_Read_Bit(void);   //读取一位
uint8_t DHT11_Check(void);  //检测DHT11
void DHT11_Rst(void);   //复位DHT11   

#endif
