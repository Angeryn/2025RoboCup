#ifndef __JY62_H
#define __JY62_H

#include <stdio.h>
#include "stm32f10x.h"                  // Device header

extern float Yaw;//실똑
extern float Wx,Wy,Wz;//실醵똑
void HWT_Init(void);
void jy62_Receive_Data(uint8_t com_data);




#endif
