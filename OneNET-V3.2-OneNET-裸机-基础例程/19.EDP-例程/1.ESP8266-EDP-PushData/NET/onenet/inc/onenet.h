#ifndef _ONENET_H_
#define _ONENET_H_





_Bool OneNet_DevLink(void);

_Bool OneNet_PushData(const char* dst_devid, const char* data, unsigned int data_len);

void OneNet_RevPro(unsigned char *cmd);

#endif
