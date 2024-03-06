/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	spi.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-08-01
	*
	*	版本： 		V1.0
	*
	*	说明： 		SPI TFT-LCD驱动
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

#include "spi.h"


SPI_InitTypeDef spi_initstruct;

/*
SPI1:	NSS---PA4
		SCK---PA5
		MISO---PA6
		MOSI---PA7

SPI2:	NSS---PB12
		SCK---PB13
		MISO---PB14
		MOSI---PB15
*/

/*
************************************************************
*	函数名称：	SPIx_Init
*
*	函数功能：	SPIx初始化
*
*	入口参数：	SPIx：SPI1或者SPI2
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void SPIx_Init(SPI_TypeDef *SPIx)
{

	GPIO_InitTypeDef gpio_initstruct;
	
	if(SPIx == SPI1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	 
		gpio_initstruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;								//复用推挽输出
		gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &gpio_initstruct);

		GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
	}
	else if(SPIx == SPI2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );
	 
		gpio_initstruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;								//复用推挽输出
		gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &gpio_initstruct);

		GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	}

	spi_initstruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	spi_initstruct.SPI_Mode = SPI_Mode_Master;							//设置SPI工作模式:设置为主SPI
	spi_initstruct.SPI_DataSize = SPI_DataSize_8b;						//设置SPI的数据大小:SPI发送接收8位帧结构
	spi_initstruct.SPI_CPOL = SPI_CPOL_High;								//选择了串行时钟的稳态:时钟悬空高
	spi_initstruct.SPI_CPHA = SPI_CPHA_2Edge;							//数据捕获于第二个时钟沿
	spi_initstruct.SPI_NSS = SPI_NSS_Soft;								//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	spi_initstruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	//定义波特率预分频的值:波特率预分频值为256
	spi_initstruct.SPI_FirstBit = SPI_FirstBit_MSB;						//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	spi_initstruct.SPI_CRCPolynomial = 7;								//CRC值计算的多项式
	SPI_Init(SPIx, &spi_initstruct);										//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPIx, ENABLE);												//使能SPI外设
	
	SPIx_ReadWriteByte(SPIx, 0xff);										//启动传输

}

/*
************************************************************
*	函数名称：	SPIx_TX_Init
*
*	函数功能：	SPIx初始化
*
*	入口参数：	SPIx：SPI1或者SPI2
*
*	返回参数：	无
*
*	说明：		只发送模式
************************************************************
*/
void SPIx_TX_Init(SPI_TypeDef *SPIx)
{

	GPIO_InitTypeDef gpio_initstruct;
	
	if(SPIx == SPI1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	 
		gpio_initstruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
		gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;						//复用推挽输出
		gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &gpio_initstruct);

		GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_7);
	}
	else if(SPIx == SPI2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );
	 
		gpio_initstruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
		gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;						//复用推挽输出
		gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &gpio_initstruct);

		GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_15);
	}

	spi_initstruct.SPI_Direction = SPI_Direction_1Line_Tx;				//设置SPI单向或者双向的数据模式:SPI设置为单线只发模式
	spi_initstruct.SPI_Mode = SPI_Mode_Master;							//设置SPI工作模式:设置为主SPI
	spi_initstruct.SPI_DataSize = SPI_DataSize_8b;						//设置SPI的数据大小:SPI发送接收8位帧结构
	spi_initstruct.SPI_CPOL = SPI_CPOL_High;								//选择了串行时钟的稳态:时钟悬空高
	spi_initstruct.SPI_CPHA = SPI_CPHA_2Edge;							//数据捕获于第二个时钟沿
	spi_initstruct.SPI_NSS = SPI_NSS_Soft;								//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	spi_initstruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//定义波特率预分频的值:波特率预分频值为256
	spi_initstruct.SPI_FirstBit = SPI_FirstBit_MSB;						//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	spi_initstruct.SPI_CRCPolynomial = 7;								//CRC值计算的多项式
	SPI_Init(SPIx, &spi_initstruct);										//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPIx, ENABLE);												//使能SPI外设

}

/*
************************************************************
*	函数名称：	SPIx_SetSpeed
*
*	函数功能：	SPI 速度设置函数
*
*	入口参数：	SPIx：SPI1或者SPI2
*				speed：速度，见说明
*
*	返回参数：	无
*
*	说明：		SPI_BaudRatePrescaler_2   2分频   (SPI 36M@sys 72M)
*				SPI_BaudRatePrescaler_8   8分频   (SPI 9M@sys 72M)
*				SPI_BaudRatePrescaler_16  16分频  (SPI 4.5M@sys 72M)
*				SPI_BaudRatePrescaler_256 256分频 (SPI 281.25K@sys 72M)
************************************************************
*/
void SPIx_SetSpeed(SPI_TypeDef *SPIx, unsigned char speed)
{
	
	spi_initstruct.SPI_BaudRatePrescaler = speed;
  	SPI_Init(SPIx, &spi_initstruct);
	
	SPI_Cmd(SPIx,ENABLE);
	
}

/*
************************************************************
*	函数名称：	SPIx_ReadWriteByte
*
*	函数功能：	读写一个字节
*
*	入口参数：	SPIx：SPI1或者SPI2
*				TxData:要写入的字节
*
*	返回参数：	读取到的字节
*
*	说明：		
************************************************************
*/
unsigned char SPIx_ReadWriteByte(SPI_TypeDef *SPIx, unsigned char TxData)
{
	
	unsigned char retry = 0;
	
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET)	//检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if(retry > 200)
			return 0;
	}
	
	SPI_I2S_SendData(SPIx, TxData);									//通过外设SPIx发送一个数据
	retry = 0;

	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET)	//检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		retry++;
		if(retry > 200)
			return 0;
	}
	
	return SPI_I2S_ReceiveData(SPIx);								//返回通过SPIx最近接收的数据
		
}

/*
************************************************************
*	函数名称：	SPIx_WriteByte
*
*	函数功能：	写一个字节
*
*	入口参数：	SPIx：SPI1或者SPI2
*				TxData:要写入的字节
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void SPIx_WriteByte(SPI_TypeDef *SPIx, unsigned char TxData)
{
	
	SPI_I2S_SendData(SPIx, TxData);									//通过外设SPIx发送一个数据
	
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);	//检查指定的SPI标志位设置与否:发送缓存空标志位
		
}
