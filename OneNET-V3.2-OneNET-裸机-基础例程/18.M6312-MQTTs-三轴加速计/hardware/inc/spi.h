#ifndef _SPI_H_
#define _SPI_H_


#include "stm32f10x.h"




void SPIx_Init(SPI_TypeDef *SPIx);

void SPIx_TX_Init(SPI_TypeDef *SPIx);

void SPIx_SetSpeed(SPI_TypeDef *SPIx, unsigned char SpeedSet);

unsigned char SPIx_ReadWriteByte(SPI_TypeDef *SPIx, unsigned char TxData);

void SPIx_WriteByte(SPI_TypeDef *SPIx, unsigned char TxData);


#endif
