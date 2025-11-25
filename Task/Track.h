#ifndef __TRACK_H
#define __TRACK_H

#include "stm32f10x.h"                  // Device header
#include "GWKJ.h"

#define BR4 GW_digital_Read_Bit(8)
#define BR3 GW_digital_Read_Bit(7)
#define BR2 GW_digital_Read_Bit(6)
#define BR1 GW_digital_Read_Bit(5)
#define BL1 GW_digital_Read_Bit(4)
#define BL2 GW_digital_Read_Bit(3)
#define BL3 GW_digital_Read_Bit(2)
#define BL4 GW_digital_Read_Bit(1)

#define FR4 GW_digital_Read_Bit2(1)
#define FR3 GW_digital_Read_Bit2(2)
#define FR2 GW_digital_Read_Bit2(3)
#define FR1 GW_digital_Read_Bit2(4)
#define FL1 GW_digital_Read_Bit2(5)
#define FL2 GW_digital_Read_Bit2(6)
#define FL3 GW_digital_Read_Bit2(7)
#define FL4 GW_digital_Read_Bit2(8)

extern uint16_t Fhd[8];//前八路灰度原始数据存放数组
extern uint16_t Bhd[8];//后八路灰度原始数据存放数组

extern int16_t F;
extern int16_t B;
extern float r;
extern float t;
void Track(float *RZ_TY);


#endif
