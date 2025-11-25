#ifndef SERVOCONTROLLER_H_
#define SERVOCONTROLLER_H_

#include "stm32f10x.h"
#include "Serial.h"

#define FRAME_HEADER 0x55             //帧头
#define CMD_SERVO_MOVE 0x03           //舵机移动指令
#define CMD_ACTION_GROUP_RUN 0x06     //运行动作组指令

typedef enum Color_Group{
	red=0,
	green=1,
	white=2,
	black=3,
	blue=4
}Color_Group;


//原始范例
//void runActionGroup(uint8_t numOfAction, uint16_t Times);

//根据新通讯协议编写：
void Servo_Move(uint8_t servoID, int16_t Angle, uint16_t Time);
void Servo_Multi_Move(uint16_t Time, int16_t Angle1, int16_t Angle2,int16_t Angle3);
void Multi_Servo_Unlock(uint16_t ID_sum, uint8_t Servo_ID1, uint8_t Servo_ID2);
//动作组函数

//根据旧通讯协议编写：
//void Servo_Move_Time_Write(uint8_t ID,int16_t Angle, uint16_t Time);
//void Servo_Move_Time_Read(uint8_t ID);
//void Servo_Move_Stop(uint8_t ID);
//void Servo_ID_Write(uint8_t ID_init,uint8_t ID_edit);
//void Servo_ID_Read(void);
//void Servo_Angle_Offset_Adjust(uint8_t ID, int16_t angle);
//void Servo_Angle_Offset_Write(uint8_t ID);
//void Servo_Angle_Offset_Read(uint8_t ID);
//void Servo_LED_Error_Write(uint8_t ID,uint8_t Num);
//void Servo_LED_Error_Read(uint8_t ID);

void Pick_color_to_space(uint8_t Color);
void Pick_abc_to_space(uint8_t ch);
void Pick_color_to_circle(uint8_t color);
void Pick_abc_to_circle(uint8_t color);





#endif
