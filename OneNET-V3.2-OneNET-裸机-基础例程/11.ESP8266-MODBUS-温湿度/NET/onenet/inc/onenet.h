#ifndef _ONENET_H_
#define _ONENET_H_





_Bool OneNet_DevLink(void);

unsigned char OneNet_SendData(unsigned short *buf ,unsigned short dataLen);

void OneNet_RevPro(unsigned char *cmd, unsigned short len);

#endif
