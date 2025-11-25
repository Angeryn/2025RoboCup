#ifndef __TASK_PUT_
#define __TASK_PUT_

#include "stm32f10x.h"                  // Device header

extern uint8_t track_status;   //循迹标志位
extern uint16_t track_status_cnt;//循迹延时标志位
extern uint16_t track_target_cnt;//循迹延时目标值

extern uint8_t position_status;//位置运动标志位
extern uint16_t position_status_cnt;
extern uint16_t position_target_cnt;

extern uint8_t Color_status;//颜色识别开启标志位
extern uint16_t Color_status_cnt;//颜色识别计数器
extern uint8_t Color_Num;//颜色计数
extern uint8_t Color_cnt;//颜色延时采集
extern unsigned char HSL[3];
extern unsigned char RGB[3];


extern uint8_t circle_position_status;//视觉定位闭环标志位
extern uint8_t abcde[5];
extern uint8_t ABC[3];

extern float xyz[3];//tx,ty,rz
extern uint32_t delaycnt;

void delay_cnt(uint32_t cnt);
uint16_t main_abs(int16_t num);
void QR_trans1(uint8_t Qrcode1);
void QR_trans2(uint8_t Qrcode2);

void position_x_open(int16_t x_target,int16_t speedx);
void position_y_open(int16_t y_target,int16_t speedy);

void position_x(int16_t x_target,int16_t speedx);
void position_x_90(int16_t x_target,int16_t speedx);
void position_y(int16_t y_target,int16_t speedy);
void position_y_90(int16_t y_target,int16_t speedy);

void position_xy(int16_t target,int16_t speedx,int16_t speedy);

void position_z(int16_t z_target,int16_t speedz);
void Turn_Angle(int16_t angle,uint16_t z_speed);

void Track_TASK1(int16_t speed);
void Lock_and_put(uint8_t color);
void PUT_TASK1(void);

void Color_Seize(void);
void Color_Seize_2(void);

void PUT_TASK2(uint8_t qrcode2);

#endif
