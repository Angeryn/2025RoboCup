#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "can.h"
#include "Emm_V5.h"
#include "Serial.h"
#include "JY62.h"
#include "Encoder.h"
#include "Timer1.h"
#include "PID.h"
#include "Key.h"
#include "PidMode.h"
#include "Math.h"
#include "mec_nav.h"
#include "GWKJ.h"
#include "Track.h"
#include "gpio.h"
#include "stdio.h"
#include "string.h"
#include "PWM.h"
#include "Serial.h"
#include "software_iic.h"
#include "ServoController.h"
#include "lcd_spi_169.h"
#include "Task_Put.h"
#include "math.h"

#define GL_Speed 1000

int16_t output[4]={1000,0,0,0};
int16_t xy[2];
float Speed[2]={0,40};
int16_t cnt=0;
int16_t anglePID_cnt=0;
float yaw_record[5]={100,100,100,100,100};
float yaw_sum=100;

unsigned char Digtal;

unsigned char rx_buff[256]={0};

//标志位定义区

uint8_t Backhome_status=0;
uint8_t record_cnt=0;

/*注：每一次视觉定位前的零点要设置为-3/0/0*/



int main(void)
{
	
	// NVIC中断优先级分组，放在main函数里配置防止重复配置
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	HWT_Init();
	PWM_Init();//气泵
	OLED_Init();
	gpio_init();//颜色传感器初始化
	GW_digital_Init();//感为灰度初始化
	board_init();
	Delay_ms(3000);
	Timer1_Init();
	
	
	
/*颜色传感器初始化*/
	while(Ping())
	{
		Delay_ms(1);
		OLED_ShowString(1,1,"Faild");
	}
	OLED_ShowString(1,1,"Succseful!");

/*调试区
//	move(output);
////	Servo_Multi_Move(300,-5,0,0);
//	
//	Servo_Multi_Move(1000,31,30,16);//一号舵机转向，到物盘上方
//	Delay_ms(1000+100);
//	Servo_Multi_Move(500,-5,0,0);//返回原点
//	Delay_ms(500+100);
//	Servo_Multi_Move(300,2,0,0);//z轴独立
//	Delay_ms(400);
//	Servo_Multi_Move(500,2,-37,0);//机械臂进行abc视觉定位的位置//-35
//	Delay_ms(600);
//	Delay_ms(400);
	Servo_Multi_Move(300,2,0,0);//机械臂回原点
	Delay_ms(400);
	Lock_and_put(5);
*/

	//出库+识别二维码
	Servo_Multi_Move(1200,0,0,0);//让机械臂变为伸展状态
	position_y(90,GL_Speed);//出库300/300,距离90000
	position_x(180,GL_Speed);//到达二维码识别点位600/300，距离180000
	while(qrcode==0)
	{
		position_x(100,100);//到达二维码识别点位
		Delay_ms(300);
		position_x(100,-100);
		Delay_ms(300);
	}
	OLED_ShowSignedNum(1,1,qrcode,3);
	QR_trans1(qrcode);

	position_x(65,600);//130/300，距离39000
	position_y(120,-400);//识别完二维码准备巡线需要多往里面进一进，165->175/175/-200
	
	//颜色识别并巡线抓取
	Color_Seize();
	
	track_status=1;
	
	while((FL4||FL3||FL2)!=0)//前左三路灰度均压线结束巡线
	{
		Track_TASK1(400);
	}
	track_status=0;

	position_z(190,200);//730/50
	position_y(186,-GL_Speed);//到达点位a距离186000
	
	PUT_TASK1();//任务一：放置
	
	//放完e点之后
	position_y(155,-GL_Speed);//左平移//改扫描二维码位置，距离159000
	position_x(108,GL_Speed);//前平移，到达二维码识别点位，距离108000
	
	qrcode=0;//二维码置0
	
	while(qrcode==0)
	{
		position_x(100,100);//到达二维码识别点位
		Delay_ms(300);
		position_x(100,-100);
		Delay_ms(300);
	}
	OLED_ShowSignedNum(1,5,qrcode,3);
	
	QR_trans2(qrcode);//转换任务二二维码
	
	position_x(75,-GL_Speed);//后退，距离75000
	position_y(55,-GL_Speed);//左平移//改扫描二维码位置，距离39000
	position_z(165,200);//旋转

	
	//任务二扫完二维码开始抓取物块
	Color_Seize_2();
	
	track_status=1;//抓完继续巡线
	record_cnt=0;
	while(Yaw>4||Yaw<-4)//前左三路灰度均压线结束巡线//快巡线
	{
		Track_TASK1(300);	
	}

	dir_control_X_Y(0,0,0,0);//停车
	track_status=0;//结束巡线
	
	position_z(91,200);//右转90°//————待更换
	position_x_90(30,-GL_Speed);//后退，以利于识别，距离30000

	
	PUT_TASK2(qrcode);
	
	position_x_90(100,-600);//后退，准备回库，距离60000
	Turn_Angle(1,200);//左转90°//————待更换确认
	position_y(670,-600);//直接进是1500//灰度进是1390
	
	while (1)
	{	
		
//		Track(Speed);
//		OLED_ShowSignedNum(2,1,Yaw,3);
//		OLED_ShowSignedNum(3,1,qrcode,3);
//		OLED_ShowSignedNum(4,1,circle_x,3);
//		OLED_ShowSignedNum(4,5,circle_y,3);
//		OLED_ShowNum(1,1,Bhd[8-1],1);
//		OLED_ShowNum(1,2,Bhd[7-1],1);
//		OLED_ShowNum(1,3,Bhd[6-1],1);
//		OLED_ShowNum(1,4,Bhd[5-1],1);
//		OLED_ShowNum(1,5,Bhd[4-1],1);
//		OLED_ShowNum(1,6,Bhd[3-1],1);
//		OLED_ShowNum(1,7,Bhd[2-1],1);
//		OLED_ShowNum(1,8,Bhd[1-1],1);
//	    OLED_ShowNum(2,1,Fhd[8-1],1);
//		OLED_ShowNum(2,2,Fhd[7-1],1);
//		OLED_ShowNum(2,3,Fhd[6-1],1);
//		OLED_ShowNum(2,4,Fhd[5-1],1);
//		OLED_ShowNum(2,5,Fhd[4-1],1);
//		OLED_ShowNum(2,6,Fhd[3-1],1);
//		OLED_ShowNum(2,7,Fhd[2-1],1);
//		OLED_ShowNum(2,8,Fhd[1-1],1);
		
//		while(PUT_TASK1(ABCDE)!=10)
//		{
//			OLED_ShowSignedNum(3,1,circle_x,3);
//			OLED_ShowSignedNum(3,5,circle_y,3);
//		}
		
		
//		if(IIC_Get_RGB(RGB,3))
//		{
//			OLED_ShowNum(2,1,RGB[0],3);
//			OLED_ShowNum(2,5,RGB[1],3);
//			OLED_ShowNum(2,9,RGB[2],3);
//			Color_Num = Color_detect(RGB);
//			OLED_ShowNum(3,1,Color_Num,1);
//		}
//		if(IIC_Get_HSL(HSL,3))
//		{
//			OLED_ShowNum(3,1,HSL[0],3);
//			OLED_ShowNum(3,5,HSL[1],3);
//			OLED_ShowNum(3,9,HSL[2],3);
//		}
		

	
	}
}




void TIM1_UP_IRQHandler(void)//根据前面的定时器配置可知，定时器刷新时间为10ms，即0.01s记一次数据
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
		if(delaycnt>0)
		{
			delaycnt--;
		}
		
		record_cnt++;
		
		if(track_status==1)//循迹模式
		{
			track_status_cnt++;
		}
		if(position_status!=0)//位置运动模式
		{
			Step_PIDAngle(xyz);
			position_status_cnt++;
		}
		if(circle_position_status == 1)//xyz闭环标志位，定圆心点位用
		{
			Step_PIDXYZ(xyz);
		}
		if(Color_status==1)//颜色传感器使能标志位
		{
			Color_status_cnt++;
		}
		

        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

