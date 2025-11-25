#include "stm32f10x.h"                  // Device header
#include "Timer1.h"
#include "encoder.h" 
#include "math.h"
#include "PID.h"

#define EPSILON 0.0001


// PID 计算函数
void PID_Calculate(PID_t *pid) 
{
	// 更新上一次误差
    pid->Err1 = pid->Err0;
    // 计算当前误差
    pid->Err0 = pid->Target - pid->Actual;

	//积分项
    if (fabs(pid->Ki)>EPSILON)//可理解为！= 0
	{
		//积分分离（一般来说对于位置控制比较有用，速度控制没太有这个问题）
		if (pid->ErrIntThreshold == 0)
		{
			pid->ErrInt += pid->Err0;
		}
		else if (fabs(pid->Err0) < pid->ErrIntThreshold)
		{
			pid->ErrInt += pid->Err0;
		}
		else
		{
			pid->ErrInt = 0;
		}
	}
	else
	{
		pid->ErrInt = 0;
	}
	
	//抗积分饱和
	if(pid->ErrInt>300000){pid->ErrInt=300000;}//积分饱和上限=PWM分辨率（1000）/ 当前Ki值
	if(pid->ErrInt<-300000){pid->ErrInt=-300000;}
	
    // 计算 PID 输出
    pid->Out = pid->Kp * pid->Err0                 //比例项
	           + pid->Ki * pid->ErrInt             //积分项
	           + pid->Kd * (pid->Err0 - pid->Err1);//微分项

    // 输出限幅
    if (pid->Out > pid->OutMax) {pid->Out = pid->OutMax;} 
	if (pid->Out < pid->OutMin) {pid->Out = pid->OutMin;}

}



///*增量式PID模板*/
//void PID_A_positon(int16_t TargetPositon_A)
//{
//	/*获取实际值*/
//	Actual = 读取传感器;//
//	
//	/*获取本次误差，上次误差和上上次误差*/
//	Error2=Error1;
//	Error1=Error0;
//	Error0=Target-Actual;
//	
//	/*PID计算*/
//	Out += Kp*(Error0-Error1)+Ki*Error0+Kd*(Error0-2*Error1+Error2);
//	
//	/*输出限幅*/
//	if(Out > MAX){Out=MAX;}
//	if(Out < MIN){Out=MIN;}
//	/*执行控制*/
//	输出至被控对象（Out）;
//}
