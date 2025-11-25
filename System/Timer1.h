#ifndef __TIMER1_H
#define __TIMER1_H

#include "stm32f10x.h"                  // Device header

extern int16_t Encoder_Count_A0;
extern int16_t Encoder_Counat_B0;
extern int16_t Encoder_Count_C0;
extern int16_t Encoder_Count_D0;

	
void Timer1_Init(void);



#endif
