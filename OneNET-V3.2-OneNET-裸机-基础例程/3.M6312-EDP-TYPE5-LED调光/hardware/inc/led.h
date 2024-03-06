#ifndef _LED_H_
#define _LED_H_





typedef struct
{

	unsigned char Led4Sta;
	unsigned char Led5Sta;
	_Bool Led6Sta;
	_Bool Led7Sta;

} LED_STATUS;

extern LED_STATUS led_status;


typedef enum
{

	LED_OFF = 0,
	LED_ON

} LED_ENUM;


void Led_Init(void);

void Led4_Set(unsigned char num);

void Led5_Set(unsigned char num);

void Led6_Set(LED_ENUM status);

void Led7_Set(LED_ENUM status);


#endif
