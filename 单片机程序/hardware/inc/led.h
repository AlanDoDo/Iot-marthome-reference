#ifndef _LED_H_    // 预处理器指令，用于防止头文件的重复包含
#define _LED_H_     


typedef struct
{

	_Bool Led_Status;

} LED_INFO;

#define LED_ON		1

#define LED_OFF	    0

extern LED_INFO led_info;


void Led_Init(void);

void Led_Set(_Bool status);


#endif
