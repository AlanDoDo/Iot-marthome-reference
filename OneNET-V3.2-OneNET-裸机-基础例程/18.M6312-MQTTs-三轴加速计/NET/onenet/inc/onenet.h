#ifndef _ONENET_H_
#define _ONENET_H_





_Bool OneNET_RegisterDevice(void);

_Bool OneNet_DevLink(void);

_Bool M6312_SendCmd(char *cmd, char *res);

void OneNet_SendData(void);

void OneNet_RevPro(unsigned char *cmd);

#endif
