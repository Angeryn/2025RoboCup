#include "stm32f10x.h"                  // Device header
#include "PID.h"
#include "Timer1.h"
#include "Encoder.h"
#include "Math.h"
#include "jy62.h"
#include "mec_nav.h"
#include "Serial.h"

/*角度环模式PID参数
 *还需在主函数中补充的参数：Target,Actual,Threshold(可暂时不补充)
 */
PID_t AnglePID={
	.Kp = 10.4,//5
	.Ki = 0.00,//015,
	.Kd = 137,//2.8
	.OutMax = 500,//la+lb
	.OutMin = -500,
	.ErrIntThreshold = 100,
	
};

PID_t Circle_X_PID={
	.Kp = 2,//5
	.Ki = 0.00,//015,
	.Kd = 0,//2.8
	.OutMax = 1000,//la+lb
	.OutMin = -1000,
	.ErrIntThreshold = 100,
	
};

PID_t Circle_Y_PID={
	.Kp = 1,//5
	.Ki = 0.00,//015,
	.Kd = 0,//2.8
	.OutMax = 1000,//la+lb
	.OutMin = -1000,
	.ErrIntThreshold = 100,
	
};




/*封装角度闭环调控函数*/
void Step_PIDAngle(float *SpeedXYZ)
{
	//写入当前位置
	AnglePID.Actual = Yaw;
	
	//PID计算
	PID_Calculate(&AnglePID);
	
	SpeedXYZ[2]= -AnglePID.Out;
	
}

/*封装X平移闭环调控函数*/
void Step_PIDXYZ(float *SpeedXYZ)
{
	//写入目标位置
//	AnglePID.Target=0;
	Circle_X_PID.Target=0;
	Circle_Y_PID.Target=0;
	
	//写入当前位置
	AnglePID.Actual = Yaw;
	Circle_X_PID.Actual = circle_y;
	Circle_Y_PID.Actual = circle_x;
	
	//PID计算
	PID_Calculate(&AnglePID);
	PID_Calculate(&Circle_X_PID);
	PID_Calculate(&Circle_Y_PID);
	
	SpeedXYZ[0]= Circle_X_PID.Out;
	SpeedXYZ[1]= -Circle_Y_PID.Out;
	SpeedXYZ[2]= -AnglePID.Out;
	
}


