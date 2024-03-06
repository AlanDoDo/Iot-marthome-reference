#ifndef _ONENET_H_
#define _ONENET_H_





_Bool OneNet_DevLink(void);

void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt);

void OneNet_Publish(const char *topic, const char *msg);

void OneNet_RevPro(unsigned char *cmd);

#endif
