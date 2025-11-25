#include "stm32f10x.h"
#include "ServoController.h"
#include "Serial.h"
#include <stdarg.h>
#include <string.h>
#include "Delay.h"
#include "Emm_V5.h"

//宏定义获得A的低八位
#define GET_LOW_BYTE(A) ((uint8_t)(A))
//宏定义获得A的高八位
#define GET_HIGH_BYTE(A) ((uint8_t)((A) >> 8))

#define DL 100

uint8_t LobotTxBuf[16];  //发送缓存//数据长度这里存疑，理论上来说大小定义为10就够了，或者干脆不定义，根据发送的大小实时定义

//2号舵机控制前后，3号控制上下

//在点位机械臂的放置位置
//	Servo_Multi_Move(1000,-2,-75,55);


/*任务一抓取函数，识别到物块颜色，抓取并放置到指定物盘位置*/
void Pick_color_to_space(uint8_t Color)
{
	Emm_V5_Pos_Control(5,1,20,20,360+640*Color,1,0);//转到物盘color号位
	Servo_Multi_Move(500,-4,0,0);//机械臂回原点
	Delay_ms(500+DL);
	Servo_Multi_Move(1000,-4,-30,72);//机械臂下放中间点
	Delay_ms(1000+DL);
	Servo_Multi_Move(500,-4,-64,98);//机械臂下放//
	Delay_ms(500+DL);
	TIM_SetCompare1(TIM3,70);//吸取物块
	Delay_ms(600);
	Servo_Multi_Move(1000,-4,0,20);//提起物块
	Delay_ms(1000+DL);
	
	Servo_Multi_Move(800,29,28,24);//一号舵机转向，到物盘上方
	Delay_ms(800+500);//这里的延时要长一点，防止物块抖动过大，抖动尚未消除直接放进去容易卡住
	Servo_Multi_Move(500,29,18,40);//垂直下探到中间点
	Delay_ms(500+DL);
	TIM_SetCompare1(TIM3,0);//放下物块
	Delay_ms(800);
	Servo_Multi_Move(500,29,30,16);//重回物盘上方
	Delay_ms(500+DL);
	Servo_Multi_Move(500,-4,0,0);//返回原点
	Delay_ms(500+DL);

}

/*任务二抓取函数，根据二维码确定按照顺序抓取的分别是什么字母*/
void Pick_abc_to_space(uint8_t ch)
{
	Emm_V5_Pos_Control(5,1,20,20,360+640*ch,1,0);//转到物盘color号位
	Servo_Multi_Move(800,-3,-30,72);//机械臂下放中间点
	Delay_ms(800+DL);
	Servo_Multi_Move(500,-3,-50,89);//机械臂下放//
	Delay_ms(500+DL);
	TIM_SetCompare1(TIM3,70);//吸取物块
	Delay_ms(600);
	Servo_Multi_Move(1000,-3,0,10);//提起物块
	Delay_ms(1000+DL);
	Servo_Multi_Move(1000,31,31,17);//一号舵机转向，到物盘上方
	Delay_ms(1200+DL);
	Servo_Multi_Move(500,31,15,47);//垂直下探
	Delay_ms(500+DL);
	TIM_SetCompare1(TIM3,0);//放下物块
	Delay_ms(800);
	Servo_Multi_Move(1000,31,30,16);//一号舵机转向，到物盘上方
	Delay_ms(1000+DL);
	Servo_Multi_Move(500,-5,0,0);//返回原点
	Delay_ms(500+DL);

}

/*任务一放置函数，抓取并放置到常规圆心位置*/
void Pick_color_to_circle(uint8_t color)
{
	Emm_V5_Pos_Control(5,1,20,40,355+color*640,1,0);//物盘转到指定位置
	Delay_ms(1000);//加入延时
	Servo_Multi_Move(1000,-4,0,0);//机械臂回原点
	Delay_ms(1000+DL);
	Servo_Multi_Move(800,31,30,16);//一号舵机转向，到物盘上方
	Delay_ms(800+DL);
	Servo_Multi_Move(800,31,3,65);//垂直下探到物盘
	Delay_ms(800+DL);
	TIM_SetCompare1(TIM3,70);//吸取物块
	Delay_ms(600);
	Servo_Multi_Move(800,24,32,16);//重回物盘上方
	Delay_ms(800+DL);
	Servo_Multi_Move(800,-4,0,0);//机械臂回原点
	Delay_ms(800+DL);
	//Servo_Multi_Move(100,-4,0,0);//z轴分离
	Servo_Multi_Move(800,-4,-74,58);//到目标点位//-2->-4//-4->-6
	Delay_ms(800+DL);
	TIM_SetCompare1(TIM3,0);//放下物块
	Delay_ms(1000);
	Servo_Multi_Move(800,-4,-40,30);//往前伸一部分
	Delay_ms(800+DL);
	Servo_Multi_Move(800,-4,0,0);//机械臂回原点
	Delay_ms(800+DL);
}

/*任务二放置函数，抓取并放置到冠亚季圆心位置*/
void Pick_abc_to_circle(uint8_t color)
{
	static int order =2;//定义为静态变量，只赋值一次
	Emm_V5_Pos_Control(5,1,20,40,380+order*640,1,0);//物盘转到指定位置//固定按照012的顺序放置
	Delay_ms(1000);//加入延时
	Servo_Multi_Move(800,2,0,0);//机械臂回原点
	Delay_ms(800+DL);
	Servo_Multi_Move(800,29,30,12);//一号舵机转向，到物盘上方
	Delay_ms(800+DL);
	Servo_Multi_Move(800,30,14,48);//垂直下探到物盘
	Delay_ms(800+DL);
	TIM_SetCompare1(TIM3,70);//吸取物块
	Delay_ms(600);
	Servo_Multi_Move(800,19,35,12);//重回物盘上方
	Delay_ms(800+DL);
	Servo_Multi_Move(800,2,-10,30);//机械臂放置中点
	Delay_ms(800+DL);
	if(color==7)//季军
	{
		Servo_Multi_Move(300,-4,-10,30);//快速进行z轴旋转，即z轴小动作分离执行
		Delay_ms(300+DL);
		Servo_Multi_Move(800,-4,-88,21);//到目标点位//-2->-4//-4->-6//-85/21
		Delay_ms(800+DL);
		TIM_SetCompare1(TIM3,0);//放下物块
		Delay_ms(1000);
		Servo_Multi_Move(800,-4,-80,18);//往前伸一部分
		Delay_ms(800+DL);
		Servo_Multi_Move(300,2,-80,18);//快速进行z轴旋转，即z轴小动作分离执行
		Delay_ms(300+DL);
	}
	else if(color==6)//亚军//待修改
	{
		Servo_Multi_Move(300,-4,-10,30);//快速进行z轴旋转，即z轴小动作分离执行
		Delay_ms(300+DL);
		Servo_Multi_Move(800,-4,-82,18);//到目标点位//-2->-4//-4->-6
		Delay_ms(800+DL);
		TIM_SetCompare1(TIM3,0);//放下物块
		Delay_ms(1000);
		Servo_Multi_Move(800,-4,-60,11);//往前伸一部分
		Delay_ms(800+DL);
		Servo_Multi_Move(300,2,-60,11);//快速进行z轴旋转，即z轴小动作分离执行
		Delay_ms(300+DL);
	}
	else if(color==5)//冠军//待修改
	{
		Servo_Multi_Move(300,-4,-10,30);//快速进行z轴旋转，即z轴小动作分离执行
		Delay_ms(300+DL);
		Servo_Multi_Move(800,-4,-80,13);//到目标点位//-2->-4//-4->-6
		Delay_ms(800+DL);
		TIM_SetCompare1(TIM3,0);//放下物块
		Delay_ms(1000);
		Servo_Multi_Move(800,-4,-62,25);//往前伸一部分
		Delay_ms(800+DL);
		Servo_Multi_Move(300,2,-62,25);//快速进行z轴旋转，即z轴小动作分离执行
		Delay_ms(300+DL);
	}
	order--;
	Servo_Multi_Move(500,2,0,0);//机械臂回原点
	Delay_ms(500+DL);//延时必不可少，否则会影响相距较近的下一点
}


//3号舵机 范围0-98

/*以下依据最新通信协议编写*/

/*************************************************
 *函数1 控制单个舵机的转动
 *指令值：3 数据长度：控制的舵机的个数*3+5
 *para1 要控制舵机的ID
 *para2 角度，可正可负，对应正反转，范围-120°~120°
 *para3 舵机转动时间，单位ms，范围0-30000ms
 ************************************************/
void Servo_Move(uint8_t servoID, int16_t Angle, uint16_t Time)
{
	if (servoID > 15 || !(Time > 0)) {  //舵机ID不能大于15时间不能小于0,可根据对应控制板修改
		return;
	}
	uint16_t Position;
	
	Position = 500 + 500*Angle/120;          //实现角度和位置的转换，这里的270°可以改为180°/360°
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;    //填充帧头0x55
	LobotTxBuf[2] = 8;                        //数据长度=要控制舵机数*3+5，此处=1*3+5
	LobotTxBuf[3] = CMD_SERVO_MOVE;           //填充舵机移动指令0x03
	LobotTxBuf[4] = 1;                        //要控制的舵机个数
	LobotTxBuf[5] = GET_LOW_BYTE(Time);       //取得时间的低八位
	LobotTxBuf[6] = GET_HIGH_BYTE(Time);      //取得时间的高八位
	LobotTxBuf[7] = servoID;                  //舵机ID
	LobotTxBuf[8] = GET_LOW_BYTE(Position);   //取得目标位置的低八位
	LobotTxBuf[9] = GET_HIGH_BYTE(Position);  //取得目标位置的高八位

	Serial2_SendArray(LobotTxBuf, 10);
}


/*************************************************
 *函数2 控制多个舵机的转动
 *舵机数为2，ID分别为1，2
 *指令值：3 数据长度：2（要控制的舵机个数）*3+5
 *para1 舵机转动时间，单位ms，范围0-30000ms
 *para2 第一个舵机的角度，可正可负，对应正反转，范围-120°~120°
 *para3 第二个舵机的角度，可正可负，对应正反转，范围-120°~120°
 ************************************************/
void Servo_Multi_Move(uint16_t Time, int16_t Angle1, int16_t Angle2,int16_t Angle3)
{
	//Angle2=-Angle2;
	uint16_t Position1,Position2,Position3;
	
	Position1 = 500 + 500*Angle1/120;           //实现角度和位置的转换
	Position2 = 500 + 500*Angle2/120;           //实现角度和位置的转换
	Position3 = 500 + 500*Angle3/120;           //实现角度和位置的转换
	/*填充数组*/
	
	LobotTxBuf[0]  = LobotTxBuf[1] = 0x55;      //填充帧头0x55
	LobotTxBuf[2]  = 14;                        //数据长度=要控制舵机数*3+5，此处=2*3+5
	LobotTxBuf[3]  = 3;                         //填充舵机移动指令0x03
	/*数组参数部分*/
	LobotTxBuf[4]  = 3;                         //要控制的舵机个数
	LobotTxBuf[5]  = GET_LOW_BYTE(Time);        //取得时间的低八位
	LobotTxBuf[6]  = GET_HIGH_BYTE(Time);       //取得时间的高八位
	LobotTxBuf[7]  = 1;                         //1号舵机ID
	LobotTxBuf[8]  = GET_LOW_BYTE(Position1);   //取得目标位置的低八位
	LobotTxBuf[9]  = GET_HIGH_BYTE(Position1);  //取得目标位置的高八位
	LobotTxBuf[10] = 2;                         //以下为2号舵机部分
	LobotTxBuf[11] = GET_LOW_BYTE(Position2);   
	LobotTxBuf[12] = GET_HIGH_BYTE(Position2);  
	LobotTxBuf[13] = 3;                         //以下为3号舵机部分
	LobotTxBuf[14] = GET_LOW_BYTE(Position3);   
	LobotTxBuf[15] = GET_HIGH_BYTE(Position3);  

	Serial2_SendArray(LobotTxBuf, 16);
}

/*************************************************
 *函数3 控制多个舵机掉电卸力(不建议使用，可能会扰乱初始位置)
 *舵机数为2，ID分别为1，2
 *指令值：20 数据长度：要控制的舵机个数+3
 *para1 要控制舵机的个数，1或2
 *para2 舵机1的ID号
 *para2 舵机2的ID号
 ************************************************/
void Multi_Servo_Unlock(uint16_t ID_sum, uint8_t Servo_ID1, uint8_t Servo_ID2)
{
	/*填充数组*/
	
	LobotTxBuf[0]  = LobotTxBuf[1] = 0x55;      //填充帧头0x55
	LobotTxBuf[2]  = (uint8_t)(ID_sum+3);       //数据长度=控制舵机数+3
	LobotTxBuf[3]  = 20;                        //填充舵机移动指令0x03
	/*数组参数部分*/
	LobotTxBuf[4]  = (uint8_t)ID_sum;           //要控制的舵机个数
	LobotTxBuf[5]  = Servo_ID1;                 //1号舵机ID
	LobotTxBuf[6] = Servo_ID2;                 //2号舵机ID

	//Serial1_SendArray(LobotTxBuf, 7);
}



/************************************************
 *               旧通信协议版本
 *该通信协议是该总线舵机自带的，用于直接控制舵机，
 *在使用舵机控制板的情况下可以不用，况且也不好用
 ***********************************************/


/*************************************************
 *函数2 读取函数1发送给舵机的时间和角度
 *数据长度：3 指令值：2 
 *para1 舵机ID号
 ************************************************/
void Servo_Move_Time_Read(uint8_t ID)
{
	uint8_t CheckSum;
	CheckSum=(~(ID+3+2))&0xFF;                 
	
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;//填充帧头0x55
	LobotTxBuf[2] = ID;                          //舵机ID
	LobotTxBuf[3] = 3;                           //数据长度,查表可知
	LobotTxBuf[4] = 2;                           //指令值
	LobotTxBuf[5] = CheckSum;                    //填充校验和
	
	//Serial1_SendArray(LobotTxBuf, 6);             //串口发送数组，大小为数据包长度
}//未测试，谨慎使用


/*************************************************
 *函数6 使舵机立即停止转动
 *数据长度：3 指令值：12 
 *para1 舵机ID号
 ************************************************/
void Servo_Move_Stop(uint8_t ID)
{
	uint8_t CheckSum;
	CheckSum=(~(ID+3+12))&0xFF;                 
	
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;//填充帧头0x55
	LobotTxBuf[2] = ID;                          //舵机ID
	LobotTxBuf[3] = 3;                           //数据长度,查表可知
	LobotTxBuf[4] = 12;                          //指令值
	LobotTxBuf[5] = CheckSum;                    //填充校验和
	
	//Serial1_SendArray(LobotTxBuf, 6);             //串口发送数组，大小为数据包长度
}//未测试，谨慎使用

/*************************************************
 *函数7（常用） 给舵机重新写入ID值，并且掉电保存
 *数据长度：4 指令值：13
 *para1 舵机原始ID号
 *para2 舵机目标ID号
 ************************************************/
void Servo_ID_Write(uint8_t ID_init,uint8_t ID_edit)
{
	uint8_t CheckSum;
	CheckSum=(~(ID_init+4+13+ID_edit))&0xFF;                 
	
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;//填充帧头0x55
	LobotTxBuf[2] = ID_init;                   //舵机原始ID
	LobotTxBuf[3] = 4;                           //数据长度,查表可知
	LobotTxBuf[4] = 13;                          //指令值
	LobotTxBuf[5] = ID_edit;                   //舵机目标ID
	LobotTxBuf[6] = CheckSum;                    //填充校验和
	
	//Serial1_SendArray(LobotTxBuf, 7);             //串口发送数组，大小为数据包长度
}//未测试，谨慎使用

/*************************************************
 *函数8（常用） 读取舵机的ID值
 *数据长度：3 指令值：14 
 ************************************************/
void Servo_ID_Read(void)
{
	uint8_t CheckSum;
	CheckSum=(~(0xFE+3+12))&0xFF;                 
	
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;//填充帧头0x55
	LobotTxBuf[2] = 0xFE;                        //不知道ID的值，填充广播ID，注意，只有该函数需要填写广播ID
	LobotTxBuf[3] = 3;                           //数据长度,查表可知
	LobotTxBuf[4] = 14;                          //指令值
	LobotTxBuf[5] = CheckSum;                    //填充校验和
	
	//Serial1_SendArray(LobotTxBuf, 6);             //串口发送数组，大小为数据包长度
}//未测试，谨慎使用

//有问题，待解决：该代码的功能是读取ID号，那为什么还要设一个ID号形参传入进去？

/*************************************************
 *函数9（常用） 舵机转动进行偏差调整              
 *数据长度：4 指令值：17 
 *para1 舵机ID号
 *para2 舵机内部的偏差值，指令到达后舵机立即进行偏差调整
 *调整范围：-125~125，对应角度-30~30
 ************************************************/
void Servo_Angle_Offset_Adjust(uint8_t ID, int16_t angle)
{
	//角度不能超出正反30°
	if (angle>30||angle<-30) 
	{  
		return;
	}
	
	uint8_t CheckSum;
	uint8_t offset=(uint8_t)angle*125/3;//偏差值与角度的转换
	CheckSum=(~(ID+4+17+offset))&0xFF;                 
	
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;//填充帧头0x55
	LobotTxBuf[2] = ID;                          //舵机ID
	LobotTxBuf[3] = 4;                           //数据长度,查表可知
	LobotTxBuf[4] = 17;                          //指令值
	LobotTxBuf[5] = offset;                      //偏差值
	LobotTxBuf[6] = CheckSum;                    //填充校验和
	
	//Serial1_SendArray(LobotTxBuf, 7);             //串口发送数组，大小为数据包长度
}//未测试，谨慎使用

/*************************************************
 *函数10（常用） 保存调整的偏差值，支持掉电保存，与函数9结合使用
 *数据长度：3 指令值：18                   
 *para1 舵机ID号
 ************************************************/
void Servo_Angle_Offset_Write(uint8_t ID)
{
	uint8_t CheckSum;
	CheckSum=(~(ID+3+18))&0xFF;                 
	
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;//填充帧头0x55
	LobotTxBuf[2] = ID;                          //舵机ID
	LobotTxBuf[3] = 3;                           //数据长度,查表可知
	LobotTxBuf[4] = 18;                          //指令值
	LobotTxBuf[5] = CheckSum;                    //填充校验和
	
	//Serial1_SendArray(LobotTxBuf, 6);             //串口发送数组，大小为数据包长度
}//未测试，谨慎使用

/*************************************************
 *函数10 读取舵机设定的偏差值
 *数据长度：3 指令值：19 
 *para1 舵机ID号
 ************************************************/
void Servo_Angle_Offset_Read(uint8_t ID)
{
	uint8_t CheckSum;
	CheckSum=(~(ID+3+19))&0xFF;                 
	
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;//填充帧头0x55
	LobotTxBuf[2] = ID;                          //舵机ID
	LobotTxBuf[3] = 3;                           //数据长度,查表可知
	LobotTxBuf[4] = 18;                          //指令值
	LobotTxBuf[5] = CheckSum;                    //填充校验和
	
	//Serial1_SendArray(LobotTxBuf, 6);             //串口发送数组，大小为数据包长度
}//未测试，谨慎使用

/*************************************************
 *函数27 写入舵机那些故障会导致LED闪烁
 *数据长度：4 指令值：35 
 *para1 舵机ID号
 *para2 报警值,可写入0-7，1过温，7过温过压和堵转
 ************************************************/
void Servo_LED_Error_Write(uint8_t ID,uint8_t Num)
{
	uint8_t CheckSum;
	CheckSum=(~(ID+4+35+Num))&0xFF;                 
	
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;//填充帧头0x55
	LobotTxBuf[2] = ID;                          //舵机ID
	LobotTxBuf[3] = 4;                           //数据长度,查表可知
	LobotTxBuf[4] = 35;                          //指令值
	LobotTxBuf[5] = Num;                         //报警值
	LobotTxBuf[6] = CheckSum;                    //填充校验和
	
	//Serial1_SendArray(LobotTxBuf, 7);             //串口发送数组，大小为数据包长度
}//未测试，谨慎使用

/*************************************************
 *函数28 读取舵机故障报警值
 *数据长度：3 指令值：36 
 *para1 舵机ID号
 ************************************************/
void Servo_LED_Error_Read(uint8_t ID)
{
	uint8_t CheckSum;
	CheckSum=(~(ID+3+36))&0xFF;                 
	
	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;//填充帧头0x55
	LobotTxBuf[2] = ID;                          //舵机ID
	LobotTxBuf[3] = 3;                           //数据长度,查表可知
	LobotTxBuf[4] = 35;                          //指令值
	LobotTxBuf[5] = CheckSum;                    //填充校验和
	
	//Serial1_SendArray(LobotTxBuf, 6);             //串口发送数组，大小为数据包长度
}//未测试，谨慎使用



//控制单个舵机运动
//参数：sevoID:舵机ID，Position:转动角度（这里以270°为例）,Time:转动时间
//void moveServo(uint8_t servoID, uint16_t Angle, uint16_t Time)
//{
//	if (servoID > 15 || !(Time > 0)) {  //舵机ID不能大于15时间不能小于0,可根据对应控制板修改
//		return;
//	}
//	uint16_t Position;
//	Position = 500 + 2000*Angle/270;          //实现角度和位置的转换，这里的270°可以改为180°/360°
//	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;    //填充帧头0x55
//	LobotTxBuf[2] = 8;                        //数据长度=要控制舵机数*3+5，此处=1*3+5
//	LobotTxBuf[3] = CMD_SERVO_MOVE;           //填充舵机移动指令0x03
//	LobotTxBuf[4] = 1;                        //要控制的舵机个数
//	LobotTxBuf[5] = GET_LOW_BYTE(Time);       //取得时间的低八位
//	LobotTxBuf[6] = GET_HIGH_BYTE(Time);      //取得时间的高八位
//	LobotTxBuf[7] = servoID;                  //舵机ID
//	LobotTxBuf[8] = GET_LOW_BYTE(Position);   //取得目标位置的低八位
//	LobotTxBuf[9] = GET_HIGH_BYTE(Position);  //取得目标位置的高八位

//	Serial1_SendArray(LobotTxBuf, 10);
//}


//控制舵机运行指定动作组
//NumOfAction:动作组序号, Times:执行次数
//Times = 0 时无限循环
//void runActionGroup(uint8_t numOfAction, uint16_t Times)
//{
//	LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;  //填充帧头
//	LobotTxBuf[2] = 5;                      	   //数据长度，此命令固定为5
//	LobotTxBuf[3] = CMD_ACTION_GROUP_RUN;          //填充运行动作组命令
//	LobotTxBuf[4] = numOfAction;                   //填充要运行的动作组号
//	LobotTxBuf[5] = GET_LOW_BYTE(Times);           //取得要运行次数的低八位
//	LobotTxBuf[6] = GET_HIGH_BYTE(Times);          //取得要运行次数的高八位

//	Serial1_SendArray(LobotTxBuf, 7);           
//}
