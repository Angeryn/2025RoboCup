#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"                  // Device header


//PID结构体
typedef struct {
	float Target;
	float Actual;
	float Out;
	
	float Err0;
	float Err1;
	float ErrInt;
	
	float ErrIntThreshold;
	
	float Kp;
	float Ki;
	float Kd;
	
	float OutMax;
	float OutMin;
} PID_t;

//PID计算函数
void PID_Calculate(PID_t *pid);



#endif
