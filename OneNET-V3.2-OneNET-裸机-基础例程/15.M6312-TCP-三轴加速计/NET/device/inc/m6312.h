#ifndef _M6312_H_
#define _M6312_H_





#define REV_OK		0	//������ɱ�־
#define REV_WAIT	1	//����δ��ɱ�־


void M6312_Init(void);

void M6312_Clear(void);

void M6312_SendData(unsigned char *data, unsigned short len);

unsigned char *M6312_GetIPD(unsigned short timeOut);


#endif
