#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f10x.h"                  // Device header


//用于重新定义当前位置
extern int16_t Encoder_Count_A;
extern int16_t Encoder_Count_B;
extern int16_t Encoder_Count_C;
extern int16_t Encoder_Count_D;

extern int16_t Encoder_Count_A0;
extern int16_t Encoder_Count_B0;
extern int16_t Encoder_Count_C0;
extern int16_t Encoder_Count_D0;

extern int16_t Val_A;
extern int16_t Val_B;
extern int16_t Val_C;
extern int16_t Val_D;

void Encoder_Init(void);

//当前位置获取函数
int16_t Encoder_A_Location(void);
int16_t Encoder_B_Location(void);
int16_t Encoder_C_Location(void);
int16_t Encoder_D_Location(void);

//当前速度获取函数
int16_t Encoder_A_Val(void);
int16_t Encoder_B_Val(void);
int16_t Encoder_C_Val(void);
int16_t Encoder_D_Val(void);

#endif
