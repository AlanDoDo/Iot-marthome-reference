/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	spi.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-08-01
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		SPI TFT-LCD����
	*
	*	�޸ļ�¼��	
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
*	�������ƣ�	SPIx_Init
*
*	�������ܣ�	SPIx��ʼ��
*
*	��ڲ�����	SPIx��SPI1����SPI2
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void SPIx_Init(SPI_TypeDef *SPIx)
{

	GPIO_InitTypeDef gpio_initstruct;
	
	if(SPIx == SPI1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	 
		gpio_initstruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
		gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;								//�����������
		gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &gpio_initstruct);

		GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
	}
	else if(SPIx == SPI2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );
	 
		gpio_initstruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;								//�����������
		gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &gpio_initstruct);

		GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	}

	spi_initstruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	spi_initstruct.SPI_Mode = SPI_Mode_Master;							//����SPI����ģʽ:����Ϊ��SPI
	spi_initstruct.SPI_DataSize = SPI_DataSize_8b;						//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	spi_initstruct.SPI_CPOL = SPI_CPOL_High;								//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	spi_initstruct.SPI_CPHA = SPI_CPHA_2Edge;							//���ݲ����ڵڶ���ʱ����
	spi_initstruct.SPI_NSS = SPI_NSS_Soft;								//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	spi_initstruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	spi_initstruct.SPI_FirstBit = SPI_FirstBit_MSB;						//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	spi_initstruct.SPI_CRCPolynomial = 7;								//CRCֵ����Ķ���ʽ
	SPI_Init(SPIx, &spi_initstruct);										//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPIx, ENABLE);												//ʹ��SPI����
	
	SPIx_ReadWriteByte(SPIx, 0xff);										//��������

}

/*
************************************************************
*	�������ƣ�	SPIx_TX_Init
*
*	�������ܣ�	SPIx��ʼ��
*
*	��ڲ�����	SPIx��SPI1����SPI2
*
*	���ز�����	��
*
*	˵����		ֻ����ģʽ
************************************************************
*/
void SPIx_TX_Init(SPI_TypeDef *SPIx)
{

	GPIO_InitTypeDef gpio_initstruct;
	
	if(SPIx == SPI1)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	 
		gpio_initstruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
		gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;						//�����������
		gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &gpio_initstruct);

		GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_7);
	}
	else if(SPIx == SPI2)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );
	 
		gpio_initstruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
		gpio_initstruct.GPIO_Mode = GPIO_Mode_AF_PP;						//�����������
		gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &gpio_initstruct);

		GPIO_SetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_15);
	}

	spi_initstruct.SPI_Direction = SPI_Direction_1Line_Tx;				//����SPI�������˫�������ģʽ:SPI����Ϊ����ֻ��ģʽ
	spi_initstruct.SPI_Mode = SPI_Mode_Master;							//����SPI����ģʽ:����Ϊ��SPI
	spi_initstruct.SPI_DataSize = SPI_DataSize_8b;						//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	spi_initstruct.SPI_CPOL = SPI_CPOL_High;								//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	spi_initstruct.SPI_CPHA = SPI_CPHA_2Edge;							//���ݲ����ڵڶ���ʱ����
	spi_initstruct.SPI_NSS = SPI_NSS_Soft;								//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	spi_initstruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	spi_initstruct.SPI_FirstBit = SPI_FirstBit_MSB;						//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	spi_initstruct.SPI_CRCPolynomial = 7;								//CRCֵ����Ķ���ʽ
	SPI_Init(SPIx, &spi_initstruct);										//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPIx, ENABLE);												//ʹ��SPI����

}

/*
************************************************************
*	�������ƣ�	SPIx_SetSpeed
*
*	�������ܣ�	SPI �ٶ����ú���
*
*	��ڲ�����	SPIx��SPI1����SPI2
*				speed���ٶȣ���˵��
*
*	���ز�����	��
*
*	˵����		SPI_BaudRatePrescaler_2   2��Ƶ   (SPI 36M@sys 72M)
*				SPI_BaudRatePrescaler_8   8��Ƶ   (SPI 9M@sys 72M)
*				SPI_BaudRatePrescaler_16  16��Ƶ  (SPI 4.5M@sys 72M)
*				SPI_BaudRatePrescaler_256 256��Ƶ (SPI 281.25K@sys 72M)
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
*	�������ƣ�	SPIx_ReadWriteByte
*
*	�������ܣ�	��дһ���ֽ�
*
*	��ڲ�����	SPIx��SPI1����SPI2
*				TxData:Ҫд����ֽ�
*
*	���ز�����	��ȡ�����ֽ�
*
*	˵����		
************************************************************
*/
unsigned char SPIx_ReadWriteByte(SPI_TypeDef *SPIx, unsigned char TxData)
{
	
	unsigned char retry = 0;
	
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET)	//���ָ����SPI��־λ�������:���ͻ���ձ�־λ
	{
		retry++;
		if(retry > 200)
			return 0;
	}
	
	SPI_I2S_SendData(SPIx, TxData);									//ͨ������SPIx����һ������
	retry = 0;

	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET)	//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	{
		retry++;
		if(retry > 200)
			return 0;
	}
	
	return SPI_I2S_ReceiveData(SPIx);								//����ͨ��SPIx������յ�����
		
}

/*
************************************************************
*	�������ƣ�	SPIx_WriteByte
*
*	�������ܣ�	дһ���ֽ�
*
*	��ڲ�����	SPIx��SPI1����SPI2
*				TxData:Ҫд����ֽ�
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void SPIx_WriteByte(SPI_TypeDef *SPIx, unsigned char TxData)
{
	
	SPI_I2S_SendData(SPIx, TxData);									//ͨ������SPIx����һ������
	
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);	//���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		
}
