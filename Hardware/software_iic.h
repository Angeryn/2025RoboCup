#include "stm32f10x.h"
#include "gw_color_sensor.h"
#include "delay.h"


#define SDA_PIN GPIO_Pin_9//新版引脚
#define SDA_PORT GPIOD
#define SCL_PIN GPIO_Pin_15
#define SCL_PORT GPIOB

/* 基本I2C操作宏 */
#define SDA_HIGH() GPIO_SetBits(SDA_PORT, SDA_PIN)
#define SDA_LOW()  GPIO_ResetBits(SDA_PORT, SDA_PIN)
#define SCL_HIGH() GPIO_SetBits(SCL_PORT, SCL_PIN)
#define SCL_LOW()  GPIO_ResetBits(SCL_PORT, SCL_PIN)
#define READ_SDA() GPIO_ReadInputDataBit(SDA_PORT, SDA_PIN)

unsigned char Ping(void);
unsigned char IIC_Get_HSL(unsigned char * Result,unsigned char len);
unsigned char IIC_Get_RGB(unsigned char * Result,unsigned char len);

