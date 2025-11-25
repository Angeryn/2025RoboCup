#ifndef __PIDMODE_H
#define __PIDMODE_H

#include "PID.h"
#include "stm32f10x.h"                  // Device header


//结构体变量——角度控制
extern PID_t AnglePID;
extern PID_t Circle_X_PID;
extern PID_t Circle_Y_PID;
//PID调控函数——角度调控
void Step_PIDAngle(float *SpeedXY);
void Step_PIDXYZ(float *SpeedXYZ);

#endif
