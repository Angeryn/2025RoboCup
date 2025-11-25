#include "software_iic.h"
#include "gpio.h"
/* 基本时序操作 */
static void IIC_Delay(void)
{
    Delay_us(10);  // 根据实际I2C速度调整延时
}
/* 软件I2C基础函数 */
void IIC_Start(void)
{
    SDA_HIGH();
    SCL_HIGH();
    IIC_Delay();
    SDA_LOW();
    IIC_Delay();
    SCL_LOW();
}

void IIC_Stop(void)
{
    SDA_LOW();
    IIC_Delay();
    SCL_HIGH();
    IIC_Delay();
    SDA_HIGH();
    IIC_Delay();
}

unsigned char IIC_WaitAck(void)
{
    unsigned char ack;
    SDA_HIGH();
    SCL_HIGH();
    IIC_Delay();
    ack = READ_SDA();
    SCL_LOW();
    IIC_Delay();
    return ack;
}

void IIC_SendAck(void)
{
    SDA_LOW();
    SCL_HIGH();
    IIC_Delay();
    SCL_LOW();
    SDA_HIGH();
}

void IIC_SendNAck(void)
{
    SDA_HIGH();
    SCL_HIGH();
    IIC_Delay();
    SCL_LOW();
}

unsigned char IIC_SendByte(unsigned char dat)
{
    for(unsigned char i = 0; i < 8; i++) {
        (dat & 0x80) ? SDA_HIGH() : SDA_LOW();
        dat <<= 1;
        SCL_HIGH();
        IIC_Delay();
        SCL_LOW();
        IIC_Delay();
    }
    return IIC_WaitAck();
}

unsigned char IIC_RecvByte(void)
{
    unsigned char dat = 0;
    SDA_HIGH();
    
    for(unsigned char i = 0; i < 8; i++) {
        dat <<= 1;
        SCL_HIGH();
        IIC_Delay();
        if(READ_SDA()) dat |= 0x01;
        SCL_LOW();
        IIC_Delay();
    }
    return dat;
}

/* 应用层函数改写 */
unsigned char IIC_ReadByte(unsigned char Salve_Address)
{
    unsigned char dat;
    
    IIC_Start();
    IIC_SendByte(Salve_Address | 0x01);  // 读模式
    dat = IIC_RecvByte();
    IIC_SendNAck();
    IIC_Stop();
    
    return dat;
}

unsigned char IIC_ReadBytes(unsigned char Salve_Address, unsigned char Reg_Address, 
                          unsigned char *Result, unsigned char len)
{
    IIC_Start();
    if(IIC_SendByte(Salve_Address & 0xFE)) {  // 写模式
        IIC_Stop();
        return 0;
    }
    if(IIC_SendByte(Reg_Address)) {
        IIC_Stop();
        return 0;
    }
    IIC_Start();
    if(IIC_SendByte(Salve_Address | 0x01)) {  // 读模式
        IIC_Stop();
        return 0;
    }
    
    for(unsigned char i = 0; i < len; i++) {
        Result[i] = IIC_RecvByte();
        (i == len-1) ? IIC_SendNAck() : IIC_SendAck();
    }
    IIC_Stop();
    return 1;
}

unsigned char IIC_WriteByte(unsigned char Salve_Address, unsigned char Reg_Address, 
                          unsigned char data)
{
    IIC_Start();
    if(IIC_SendByte(Salve_Address & 0xFE)) {  // 写模式
        IIC_Stop();
        return 0;
    }
    if(IIC_SendByte(Reg_Address)) {
        IIC_Stop();
        return 0;
    }
    if(IIC_SendByte(data)) {
        IIC_Stop();
        return 0;
    }
    IIC_Stop();
    return 1;
}

unsigned char IIC_WriteBytes(unsigned char Salve_Address, unsigned char Reg_Address,
                           unsigned char *data, unsigned char len)
{
    IIC_Start();
    if(IIC_SendByte(Salve_Address & 0xFE)) {
        IIC_Stop();
        return 0;
    }
    if(IIC_SendByte(Reg_Address)) {
        IIC_Stop();
        return 0;
    }
    
    for(unsigned char i = 0; i < len; i++) {
        if(IIC_SendByte(data[i])) {
            IIC_Stop();
            return 0;
        }
    }
    IIC_Stop();
    return 1;
}
unsigned char Ping(void)
{
	unsigned char dat;
	IIC_ReadBytes(Color_Adress<<1,PING,&dat,1);
	if(dat==PING_OK)
	{
			return 0;
	}	
	else return 1;
}
unsigned char IIC_Get_Error(void)
{
	unsigned char dat;
	IIC_ReadBytes(Color_Adress<<1,Error,&dat,1);
	return dat;
}
unsigned char IIC_Get_RGB(unsigned char * Result,unsigned char len)
{
	if(IIC_ReadBytes(Color_Adress<<1,RGB_Reg,Result,len))return 1;
	else return 0;
}
unsigned char IIC_Get_HSL(unsigned char * Result,unsigned char len)
{
	if(IIC_ReadBytes(Color_Adress<<1,HSL_Reg,Result,len))return 1;
	else return 0;
}


//void setup(void)
//{
//	gpio_init();//初始化I2C
//	Ping();//ping函数，用于同步esp32与灰度传感器。
//	IIC_Start();
//	
//	//写命令
//	IIC_Start();//初始化函数
//	IIC_SendByte(Color_Adress<<1);//起始位函数。
//	Wire.write(0xD0);//写函数，发送命令0xB0。
//	Wire.endTransmission(1);//停止位函数，写入1为真，即产生停止位。
//}

//void Setup(void)
//{
//	IIC_Start();
//	Ping();
//	
//}

