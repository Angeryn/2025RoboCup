#include "stm32f10x.h"                  // Device header
#include "mec_nav.h"
#include "jy62.h"
#include "math.h"
#include "Emm_V5.h"
#define PI 3.14159265358979323846f

//当前底盘a（左右轮轴一半距离）：78.7
#define la 78.7
//当前底盘b（前后轮轴一半距离）：60
#define lb 60

// 麦轮参数
float MEC_WHEEL_PARAMETER = 1;  // 轮子半径，决定轮子一圈的行程
float VEC_SPD_MAX = 500;         // 矢量速度最大值
float TURN_SPD_MAX = 200;        // 矢量转向最大值


// 全局变量
mec_spd_param MEC_SPEED[3] = {0};  // 麦轮转速
mec_DIR_param MEC_SPD_S = {0};     // 速度矢量
nav_loc_param TAGET_LOC = {0};     // 目标坐标
nav_loc_param current_loc[3] = {0};// 全局坐标，当前，上次，上上次

//目标点存放二维数组，第一位目标点数量，第二位存放目标点位置及航向
float ZB_TO_GO[10][3];

// 限制浮点数范围
float limit_flt(float value, float max, float min) 
{
    if (value > max) {
        return max;
    } else if (value < min) {
        return min;
    }
    return value;
}

/*
 *开环模式，直接根据速度分量和转向控制麦轮，一般用于测试
 *包含平移，旋转动作模式
 */
void dir_control_X_Y(float x, float y, float th, uint8_t acc) 
{
    float v_tx = x;
    float v_ty = y;
    float omega = th;
    float v_w[4];
    v_w[0] = v_ty + v_tx - omega;//*(la+lb);
    v_w[1] = v_ty - v_tx + omega;//*(la+lb);
    v_w[2] = v_ty - v_tx - omega;//*(la+lb);
    v_w[3] = v_ty + v_tx + omega;//*(la+lb);
	
    int16_t output[4];
    output[0] = (int16_t)v_w[0] ;
    output[1] = (int16_t)v_w[1] ;
    output[2] = (int16_t)v_w[2] ;
    output[3] = (int16_t)v_w[3] ;

//    MEC_SPEED[0].A = v_w[0];
//    MEC_SPEED[0].B = v_w[1];
//    MEC_SPEED[0].C = v_w[2];
//    MEC_SPEED[0].D = v_w[3];

    move(output);
}



// 麦克纳姆轮正运动学姿态解算，全局坐标
// 输入电机速度（包含提供角速度的部分和提供线速度的部分）
// 输出最终位置和角度（航向角）
void MEC_LOC_CACULATE(void) 
{
    nav_loc_param DIS_TO_ADD = {0};
    DIS_TO_ADD.angle = Yaw;  // 应该带入的角度

    float cos_angle = cosf(DIS_TO_ADD.angle / 180 * PI);
    float sin_angle = sinf(DIS_TO_ADD.angle / 180 * PI);
    float cos_angle_90 = cosf((DIS_TO_ADD.angle + 90) / 180 * PI);
    float sin_angle_90 = sinf((DIS_TO_ADD.angle + 90) / 180 * PI);

    float term1 = (MEC_SPEED[0].C + MEC_SPEED[0].D + MEC_SPEED[1].A + MEC_SPEED[1].B) / 2 * MEC_WHEEL_PARAMETER;
    float term2 = (MEC_SPEED[0].C - MEC_SPEED[0].B + MEC_SPEED[1].A - MEC_SPEED[1].D) / 2 * MEC_WHEEL_PARAMETER;

    DIS_TO_ADD.loc.x = term1 * cos_angle + term2 * cos_angle_90;
    DIS_TO_ADD.loc.y = term1 * sin_angle + term2 * sin_angle_90;

    current_loc[0].loc.x += DIS_TO_ADD.loc.x * 0.01 * 0.2;
    current_loc[0].loc.y += DIS_TO_ADD.loc.y * 0.01 * 0.2;
    current_loc[0].angle = Yaw;
}

// 麦克纳姆轮逆运动学姿态解算，输入方向角，速度大小，逆时针方向特别注意，计算转速
// 输入最终位置和角度（航向角）
// 输出电机速度（包含提供角速度的部分和提供线速度的部分）
void dir_control(mec_DIR_param *SPD, uint8_t fa) 
{
    SPD->turn = limit_flt(SPD->turn, TURN_SPD_MAX, -TURN_SPD_MAX);  // 速度限幅
    SPD->data = limit_flt(SPD->data, VEC_SPD_MAX, -VEC_SPD_MAX);

    float vx = SPD->data * cosf(SPD->ang * PI / 180);
    float vy = SPD->data * sinf(SPD->ang * PI / 180);

    float v_tx = vx;
    float v_ty = vy;
    float omega = SPD->turn;

    float v_w[4];
    v_w[0] = v_tx - v_ty - omega;
    v_w[1] = v_tx + v_ty + omega;
    v_w[2] = v_tx + v_ty - omega;
    v_w[3] = v_tx - v_ty + omega;

    int16_t output[4];
    output[0] = -(int16_t)(v_w[0] * 1);
    output[1] = (int16_t)(v_w[1] * 1);
    output[2] = -(int16_t)(v_w[2] * 1);
    output[3] = (int16_t)(v_w[3] * 1);

    MEC_SPEED[0].A = v_w[1];
    MEC_SPEED[0].B = v_w[0];
    MEC_SPEED[0].C = v_w[2];
    MEC_SPEED[0].D = v_w[3];

    move(output);
}

// 移动函数，输入坐标，角度
uint8_t move_pos(nav_loc_param *target) 
{
    float DIS_TO_GO = fabs(target->loc.x - current_loc[0].loc.x) + fabs(target->loc.y - current_loc[0].loc.y);
    float ANG_TO_T = target->angle - Yaw;
    float X_ = target->loc.x - current_loc[0].loc.x;
    float Y_ = target->loc.y - current_loc[0].loc.y;

    if (Y_ == 0) {
        MEC_SPD_S.ang = (X_ >= 0) ? 90 - Yaw : -90 - Yaw;
    } else if (X_ >= 0 && Y_ > 0) {
        MEC_SPD_S.ang = atanf(Y_ / X_) * 180 / PI - Yaw;
    } else if (X_ < 0 && Y_ >= 0) {
        MEC_SPD_S.ang = 90 + atanf(Y_ / -X_) * 180 / PI - Yaw;
    } else if (X_ < 0 && Y_ < 0) {
        MEC_SPD_S.ang = 180 + atanf(-Y_ / -X_) * 180 / PI - Yaw;
    } else if (X_ >= 0 && Y_ < 0) {
        MEC_SPD_S.ang = 270 + atanf(-Y_ / X_) * 180 / PI - Yaw;
    }

    MEC_SPD_S.turn = 10 * ANG_TO_T;
    MEC_SPD_S.data = 1 * DIS_TO_GO;

    if (DIS_TO_GO > 5 || fabs(ANG_TO_T) > 2) {
        dir_control(&MEC_SPD_S, 250);
        return 1;
    } else {
        MEC_SPD_S.ang = 0;
        MEC_SPD_S.data = 0;
        MEC_SPD_S.turn = 0;
        dir_control(&MEC_SPD_S, 250);
        return 0;
    }
}

// 移动到指定坐标和角度
void MOVE_TILL_D(float x, float y, float a) 
{
    TAGET_LOC.loc.x = x;
    TAGET_LOC.loc.y = y;
    TAGET_LOC.angle = a;
    while (move_pos(&TAGET_LOC) == 1) 
	{
        // 等待到达目标位置
    }
}

// 移动到指定数组中的坐标和角度
void MOVE_TILL_DDDD(int x) 
{
    TAGET_LOC.loc.x = ZB_TO_GO[x][0];
    TAGET_LOC.loc.y = ZB_TO_GO[x][1];
    TAGET_LOC.angle = ZB_TO_GO[x][2];
    while (move_pos(&TAGET_LOC) == 1) {
        // 等待到达目标位置
    }
}

