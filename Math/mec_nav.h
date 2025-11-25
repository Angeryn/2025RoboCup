#ifndef _mec_nav_h
#define _mec_nav_h
#include "stm32f10x.h"                  // Device header



typedef struct _point_F_
{
    float x;
    float y;
}point_F;

/*坐标，角度*/
typedef struct _nav_loc_param_
{
    point_F loc;
    float angle;
}nav_loc_param;

// 麦轮转速结构体
typedef struct {
    float A;
    float B;
    float C;
    float D;
} mec_spd_param;

// 速度矢量结构体
typedef struct {
    float data;  // 线速度
    float ang;   // 角度
    float turn;  // 角速度
} mec_DIR_param;



void dir_control_X_Y(float x, float y,float a,uint8_t fa);

void dir_control(mec_DIR_param *SPD,uint8_t fa);
void MEC_LOC_CACULATE(void);
uint8_t move_pos(nav_loc_param *taget);

void MOVE_TILL_D(float x,float y,float a);
void MOVE_TILL_DDDD(int x);


extern nav_loc_param TAGET_LOC;
extern nav_loc_param current_loc[3];
extern mec_spd_param MEC_SPEED[3];
extern mec_DIR_param MEC_SPD_S;/*速度矢量*/
extern float VEC_SPD_MAX;

#endif
