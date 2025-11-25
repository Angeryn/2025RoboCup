#include "stm32f10x.h"                  // Device header
#include "PID.h"
#include "mec_nav.h"
#include "PidMode.h"
#include "Task_Put.h"                 
#include "Track.h"
#include "ServoController.h"
#include "OLED.h"
#include "Delay.h"
#include "Emm_V5.h"
#include "Serial.h"
#include "software_iic.h"
#include "gpio.h"
#include "JY62.h"


//0，1，2，3，4分别对应红0、绿1、白2、黑3、蓝4
#define Red 0
#define Green 1
#define White 2
#define Black 3
#define Blue 4

#define A 5
#define B 6
#define C 7

#define back_speed -600
#define back_DL 5

uint8_t track_status = 0;   //循迹标志位
uint16_t track_status_cnt=0;//循迹延时标志位
uint16_t track_target_cnt=0;//循迹延时目标值

uint8_t position_status = 0;//位置运动标志位
uint16_t position_status_cnt=0;//位置延时标志位
uint16_t position_target_cnt=0;//位置延时目标值

uint8_t Color_status=0;//颜色识别开启标志位
uint16_t Color_status_cnt=0;//颜色识别计数器

uint8_t Color_Num=5;//颜色计数
uint8_t Color_cnt=0;//颜色延时采集

int color_i=0;//用于任务二

unsigned char HSL[3]={0};
unsigned char RGB[3]={0};

//标志位定义
uint8_t circle_position_status=0;//视觉定位闭环标志位

//定义字母数组
uint8_t abcde[5]={0,1,2,3,4};//0,1,2,3,4分别对应红、绿、白、黑、蓝，实际字母数组要根据扫描到的二维码决定
uint8_t ABC[3]={5,6,7};//任务二，0，1，2分别对应ABC，即冠亚季


float xyz[3]={0,0,0};//tx,ty,rz
uint32_t delaycnt=0;
float RZ_TY[2]={0,0};//循迹速度数组


//定义颜色数组
enum Color_Group color;

//非阻塞delay，可以在内部加任务
void delay_cnt(uint32_t cnt)
{
	delaycnt = cnt;
	
	while(delaycnt>0)
	{
		Track_TASK1(200);
	}
	

}
//绝对值函数
uint16_t main_abs(int16_t num)
{
	uint16_t outcome=0;
	outcome=num>=0?num:-num;
	return outcome;
}

//任务1二维码转换，0，1，2，3，4分别对应红0、绿1、白2、黑3、蓝4
void QR_trans1(uint8_t Qrcode1)
{
	if(Qrcode1==1)
	{
		abcde[0]=Black;
		abcde[1]=White;
		abcde[2]=Red;
		abcde[3]=Green;
		abcde[4]=Blue;
	}
	else if(Qrcode1==2)
	{
		abcde[0]=White;
		abcde[1]=Black;
		abcde[2]=Red;
		abcde[3]=Green;
		abcde[4]=Blue;
	}
	else if(Qrcode1==3)
	{
		abcde[0]=White;
		abcde[1]=Black;
		abcde[2]=Green;
		abcde[3]=Red;
		abcde[4]=Blue;
	}
	else if(Qrcode1==4)
	{
		abcde[0]=Blue;
		abcde[1]=White;
		abcde[2]=Black;
		abcde[3]=Red;
		abcde[4]=Green;
	}
	else if(Qrcode1==5)
	{
		abcde[0]=White;
		abcde[1]=Red;
		abcde[2]=Blue;
		abcde[3]=Black;
		abcde[4]=Green;
	}
	else if(Qrcode1==6)
	{
		abcde[0]=Black;
		abcde[1]=Red;
		abcde[2]=Blue;
		abcde[3]=White;
		abcde[4]=Green;
	}
	else if(Qrcode1==7)
	{
		abcde[0]=Blue;
		abcde[1]=Green;
		abcde[2]=Black;
		abcde[3]=White;
		abcde[4]=Red;
	}
	else if(Qrcode1==8)
	{
		abcde[0]=Green;
		abcde[1]=White;
		abcde[2]=Blue;
		abcde[3]=Black;
		abcde[4]=Red;
	}
	else if(Qrcode1==9)
	{
		abcde[0]=White;
		abcde[1]=Green;
		abcde[2]=Black;
		abcde[3]=Blue;
		abcde[4]=Red;
	}
	else if(Qrcode1==10)
	{
		abcde[0]=Black;
		abcde[1]=Red;
		abcde[2]=Blue;
		abcde[3]=Green;
		abcde[4]=White;
	}
	else if(Qrcode1==11)
	{
		abcde[0]=Red;
		abcde[1]=Blue;
		abcde[2]=Green;
		abcde[3]=Black;
		abcde[4]=White;
	}
	else if(Qrcode1==12)
	{
		abcde[0]=Green;
		abcde[1]=Red;
		abcde[2]=Black;
		abcde[3]=Blue;
		abcde[4]=White;
	}
	else if(Qrcode1==13)
	{
		abcde[0]=White;
		abcde[1]=Red;
		abcde[2]=Blue;
		abcde[3]=Green;
		abcde[4]=Black;
	}
	else if(Qrcode1==14)
	{
		abcde[0]=Red;
		abcde[1]=Green;
		abcde[2]=White;
		abcde[3]=Blue;
		abcde[4]=Black;
	}
	else if(Qrcode1==15)
	{
		abcde[0]=Blue;
		abcde[1]=White;
		abcde[2]=Green;
		abcde[3]=Red;
		abcde[4]=Black;
	}
	else if(Qrcode1==16)
	{
		abcde[0]=Green;
		abcde[1]=Blue;
		abcde[2]=Red;
		abcde[3]=White;
		abcde[4]=Black;
	}
}
//任务二二维码转换
void QR_trans2(uint8_t Qrcode2)
{
	if(Qrcode2==1)
	{
		ABC[0]=A;
		ABC[1]=B;
		ABC[2]=C;
	}
	else if(Qrcode2==2)
	{
		ABC[0]=A;
		ABC[1]=C;
		ABC[2]=B;
	}
	else if(Qrcode2==3)
	{
		ABC[0]=B;
		ABC[1]=A;
		ABC[2]=C;
	}
	else if(Qrcode2==4)
	{
		ABC[0]=B;
		ABC[1]=C;
		ABC[2]=A;
	}
	else if(Qrcode2==5)
	{
		ABC[0]=C;
		ABC[1]=A;
		ABC[2]=B;
	}
	else if(Qrcode2==6)
	{
		ABC[0]=C;
		ABC[1]=B;
		ABC[2]=A;
	}
}

//x开环位置
void position_x_open(int16_t x_target,int16_t speedx)
{
	position_status=1;
	position_status_cnt=0;//距离计数清零
	while(position_status_cnt < x_target)
	{
		dir_control_X_Y(speedx,0,0,0);
	}
	dir_control_X_Y(0,0,0,0);
	position_status=0;//关闭距离定位
	position_status_cnt=0;//距离计数清零
}

//y角度开环位置
void position_y_open(int16_t y_target,int16_t speedy)
{
	position_status=1;
	position_status_cnt=0;//距离计数清零
	while(position_status_cnt < y_target)
	{
		dir_control_X_Y(0,speedy,0,0);
	}
	dir_control_X_Y(0,0,0,0);
	position_status=0;//关闭距离定位
	position_status_cnt=0;//距离计数清零
}

//x角度闭环位置
void position_x(int16_t x_target,int16_t speedx)
{
	position_status=1;
	position_status_cnt=0;//距离计数清零
	AnglePID.Target=0;
	xyz[0] = speedx;//设置速度
	while(position_status_cnt < x_target)
	{
		dir_control_X_Y(xyz[0],0,xyz[2],0);
	}
	dir_control_X_Y(0,0,0,0);
	position_status=0;//关闭距离定位
	position_status_cnt=0;//距离计数清零
	//速度清零
	xyz[0] = 0;
	xyz[1] = 0;
	xyz[2] = 0;
}

//x角度闭环位置,固定90
void position_x_90(int16_t x_target,int16_t speedx)
{
	position_status=1;
	position_status_cnt=0;//距离计数清零
	AnglePID.Target=-90;
	xyz[0] = speedx;//设置速度
	while(position_status_cnt < x_target)
	{
		dir_control_X_Y(xyz[0],0,xyz[2],0);
	}
	dir_control_X_Y(0,0,0,0);
	position_status=0;//关闭距离定位
	position_status_cnt=0;//距离计数清零
	//速度清零
	xyz[0] = 0;
	xyz[1] = 0;
	xyz[2] = 0;
}
//y角度闭环位置,固定0
void position_y(int16_t y_target,int16_t speedy)
{
	position_status=1;
	position_status_cnt=0;//距离计数清零
	AnglePID.Target=0;
	xyz[1] = speedy;//设置速度
	while(position_status_cnt < y_target)
	{
		dir_control_X_Y(0,xyz[1],xyz[2],0);
	}
	dir_control_X_Y(0,0,0,0);
	position_status=0;//关闭距离定位
	position_status_cnt=0;//距离计数清零
	//速度清零
	xyz[0] = 0;
	xyz[1] = 0;
	xyz[2] = 0;
}

//y角度闭环位置,固定90°
void position_y_90(int16_t y_target,int16_t speedy)
{
	position_status=1;
	position_status_cnt=0;//距离计数清零
	AnglePID.Target=-90;
	xyz[1] = speedy;//设置速度
	while(position_status_cnt < y_target)
	{
		dir_control_X_Y(0,xyz[1],xyz[2],0);
	}
	dir_control_X_Y(0,0,0,0);
	position_status=0;//关闭距离定位
	position_status_cnt=0;//距离计数清零
	//速度清零
	xyz[0] = 0;
	xyz[1] = 0;
	xyz[2] = 0;
}

//xy角度闭环位置,固定0°
void position_xy(int16_t target,int16_t speedx,int16_t speedy)
{
	position_status=1;
	position_status_cnt=0;//距离计数清零
	AnglePID.Target=0;
	xyz[1] = speedy;//设置速度
	xyz[0] = speedx;
	while(position_status_cnt < target)
	{
		dir_control_X_Y(xyz[0],xyz[1],xyz[2],0);
	}
	dir_control_X_Y(0,0,0,0);
	position_status=0;//关闭距离定位
	position_status_cnt=0;//距离计数清零
	//速度清零
	xyz[0] = 0;
	xyz[1] = 0;
	xyz[2] = 0;
}
//z角度开环位置
void position_z(int16_t z_target,int16_t speedz)
{
	position_status=1;
	position_status_cnt=0;//距离计数清零
	while(position_status_cnt < z_target)
	{
		dir_control_X_Y(0,0,speedz,0);
	}
	dir_control_X_Y(0,0,0,0);

}
//z角度闭环转向
void Turn_Angle(int16_t angle,uint16_t z_speed)
{
	AnglePID.Target = angle;
	AnglePID.OutMax = z_speed;
	AnglePID.OutMin = -z_speed;
	position_status=1;//开启位置调控模式
	while((Yaw-angle)<-1||(Yaw-angle)>1)
	{
		OLED_ShowSignedNum(3,1,Yaw,3);
		dir_control_X_Y(0,0,-AnglePID.Out,0);
	}
	position_status=0;//关闭位置模式
	dir_control_X_Y(0,0,0,0);
	AnglePID.OutMax = 500;//恢复默认速度限幅
	AnglePID.OutMin = -500;
}

void Track_TASK1(int16_t speed)
{
    if((track_status==1)&&(track_status_cnt>4))//每40ms执行巡线程序一次
	{
		Track(RZ_TY);
		dir_control_X_Y(speed,RZ_TY[1],RZ_TY[0],0);
		track_status_cnt = 0;
	}
}

//任务一颜色识别加抓取
void Color_Seize(void)
{
	track_status=1;//开启循迹
	Color_status=1;//开启颜色识别
	while(Color_status!=0)
	{
		Track_TASK1(450);//循迹模式,置1开启
		
		if(Color_status_cnt>10&&Color_status==1)//颜色传感器识别，50ms执行一次
		{
			if(IIC_Get_RGB(RGB,3))
			{
				OLED_ShowNum(2,1,RGB[0],3);
				OLED_ShowNum(2,5,RGB[1],3);
				OLED_ShowNum(2,9,RGB[2],3);

				//判断是否识别到目标五个颜色之一
				if(RGB[0]>0||RGB[1]>0||RGB[2]>0)
				{
					Color_Num = Color_detect(RGB);
					if(Color_Num==3)//如果识别成黑色
					{
						while(IIC_Get_HSL(HSL,3)==0);//等待成功获取HSL值
						if(HSL[0]>200)//检测是否白色被错误识别成黑色
						{
							Color_Num=5;
						}
						else
						{
							Color_Num=3;
						}
					}
					OLED_ShowNum(3,1,Color_Num,1);
					if(Color_Num!=5)
					{
						delay_cnt(120);//识别到了先走一会
						dir_control_X_Y(0,0,0,0);//停车
						Delay_ms(100);
						Pick_color_to_space(Color_Num);//抓取
						Color_Num=5;//重置颜色
						Color_cnt++;
					}
				}
			}
			Color_status_cnt=0;
		}
		if(Color_cnt==5)//抓完五个物块
		{
			Color_status=0;//颜色识别标志位置0
		}
	}
	track_status=0;//关闭循迹
}

//任务二识别加抓取
void Color_Seize_2(void)
{
	track_status=1;//开启循迹
	Color_status=1;//开启颜色识别
	while(Color_status!=0)
	{
		Track_TASK1(450);//循迹模式,置1开启
		
		if(Color_status_cnt>10&&Color_status==1)//颜色传感器识别，50ms执行一次
		{
			if(IIC_Get_RGB(RGB,3))
			{
				OLED_ShowNum(2,1,RGB[0],3);
				OLED_ShowNum(2,5,RGB[1],3);
				OLED_ShowNum(2,9,RGB[2],3);

				//判断是否识别到目标五个颜色之一
				if(RGB[0]>0||RGB[1]>0||RGB[2]>0)
				{
					Color_Num = Color_detect(RGB);
					
					OLED_ShowNum(3,1,Color_Num,1);
					
					if(Color_Num==2)//任务二只需要识别白色
					{
						delay_cnt(150);//识别到了先走一会
						dir_control_X_Y(0,0,0,0);//停车
						Delay_ms(100);
						Pick_abc_to_space(color_i);//按顺序抓取放置，储物格0,1,2放置3，2，1取决于它的先后顺序，这个先后顺序机器人是通过二维码知道的
						Color_Num=5;//重置颜色
						color_i++;
					}
				}
			}
			Color_status_cnt=0;
		}
		if(color_i==3)//抓完三个物块
		{
			Color_status=0;//颜色识别标志位置0
		}
	}
	track_status=0;//关闭循迹
}

//任务一二视觉锁定并放置函数
void Lock_and_put(uint8_t color)
{
	if(color<=4)//任务一,0<=color<=4
	{
		Servo_Multi_Move(300,2,0,0);//z轴独立
		Delay_ms(400);
		Servo_Multi_Move(500,2,-11,0);//机械臂进行color视觉定位的位置
		Delay_ms(600);
	}
	else if(color>=5&&color<=7)
	{
		Servo_Multi_Move(300,2,0,0);//z轴独立
		Delay_ms(400);
		Servo_Multi_Move(500,2,-37,0);//机械臂进行abc视觉定位的位置//-35
		Delay_ms(600);
	}
	circle_position_status=1;//开启视觉定位
	while(main_abs(circle_x)>0||main_abs(circle_y)>0)
	{
		dir_control_X_Y(xyz[0],xyz[1],xyz[2],0);
	}
	dir_control_X_Y(0,0,0,0);//关闭闭环的同时电机速度置0
	
	circle_position_status=0;
	Delay_ms(200);//100
	
	if(color<=4)//0<=color<=4
	{
		Pick_color_to_circle(color);//任务一放置
	}
	else
	{
		Pick_abc_to_circle(color);//任务二放置 
	}
	
}


//任务一放置
void PUT_TASK1(void)
{
	//放置a
	Lock_and_put(abcde[0]);//将a点颜色赋给color
	//a->b//x-200y-280,长边L344，x/L=0.581,y/L=0.813单位mm
	position_xy(81,-581,-813);//后两位是在合成速度为1000的情况下xy的速度分量
	
	//放置b
	Lock_and_put(abcde[1]);//将b点颜色赋给color
	//b->c//x-646y280,长边Ll704，x/L=0.917,y/L=0.397单位mm
	position_xy(164,-917,397);//后两位是在合成速度为1000的情况下xy的速度分量
	
	
	//放置c
	Lock_and_put(abcde[2]);//将c点颜色赋给color
	//c->d//x-110y-280,长边L300，x/L=0.367,y/L=0.933单位mm
	position_xy(65,-367,-933);//后两位是在合成速度为800的情况下xy的速度分量120/-220/-560
	
	//放置d//
	Lock_and_put(abcde[3]);//将d点颜色赋给color
	//d->e//x-500y70,长边L505，x/L=0.990,y/L=0.138单位mm
	position_xy(130,-990,138);//后两位是在合成速度为1000的情况下xy的速度分量
	
	//放置e
	Lock_and_put(abcde[4]);//将e点颜色赋给color
	
}

void TASK2_move(uint16_t length, int16_t speed, uint8_t ch)
{
	position_y_90(length,speed);//length为移动距离，speed为移动速度包含方向
	AnglePID.Target=-90;
	Lock_and_put(ch);//放置ch，即5/6/7
}
/*
距离参考：7（季军）<--230-->5（冠军）<--230-->6（亚军）<--270-->出发点
*/
void PUT_TASK2(uint8_t qrcode2)//注意由于放进去的时候是按照321的顺序放进去的，所以取得时候是按照123的顺序取出来，即都是765
{

	if(qrcode2==1)//ABC//任务二二维码
	{
		TASK2_move(250,-600,5);//A
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		TASK2_move(115,600,6);//B
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		TASK2_move(230,-600,7);//C
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		//最后应该再加一个从冠军返回的动作
		position_y_90(95,600);//回到冠军点准备回库//115
		AnglePID.Target=-90;
	}
	else if(qrcode2==2)//ACB//任务二二维码
	{
		TASK2_move(250,-600,5);//A
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		TASK2_move(115,-600,7);//C
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		TASK2_move(230,600,6);//B
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		//最后应该再加一个从冠军返回的动作
		position_y_90(135,-600);//回到冠军点准备回库
		AnglePID.Target=-90;
	}
	else if(qrcode2==3)//BAC//任务二二维码
	{
		TASK2_move(135,-600,6);//B
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		TASK2_move(115,-600,5);//A
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		TASK2_move(115,-600,7);//C
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		//最后应该再加一个从冠军返回的动作
		position_y_90(95,600);//回到冠军点准备回库//115
		AnglePID.Target=-90;
	}
	else if(qrcode2==4)//BCA//任务二二维码
	{
		TASK2_move(150,-600,6);//B
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		TASK2_move(230,-600,7);//C
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		TASK2_move(115,600,5);//A
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		
	}
	else if(qrcode2==5)//CAB//任务二二维码
	{
		TASK2_move(365,-600,7);//C
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		TASK2_move(115,600,5);//A
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		TASK2_move(115,600,6);//B
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		//最后应该再加一个从冠军返回的动作
		position_y_90(135,-600);//回到冠军点准备回库
		AnglePID.Target=-90;
	}
	else if(qrcode2==6)//CAB//任务二二维码
	{
		TASK2_move(365,-600,7);//C
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		TASK2_move(230,600,6);//B
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		TASK2_move(115,-600,5);//A
		position_x_90(back_DL,back_speed);//退一步防止撞到台子
		
	}
	
}
