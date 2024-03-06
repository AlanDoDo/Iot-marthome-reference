#ifndef _HWTIMER_H_
#define _HWTIMER_H_

#include "stm32f10x.h"



typedef struct
{

	unsigned char timer7Out;

} TIM_INFO;

extern TIM_INFO timer_info;


void Timer1_8_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc);

void Timer6_7_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc);


#endif
