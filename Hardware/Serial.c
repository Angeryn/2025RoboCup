#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include <Delay.h>
uint8_t Serial1_TxPacket[100];				//FF 01 02 03 04 FE
uint8_t Serial1_RxPacket[100];
uint8_t Serial1_RxFlag;

uint8_t Serial3_TxPacket[20];				//FF 01 02 03 04 FE
uint8_t Serial3_RxPacket[20];
uint8_t Serial3_RxFlag;

int16_t sum_circle=0,circle_x=100,circle_y=100,r_=0;
uint16_t qrcode=0;
void Serial_Init(void)
{
    // Enable USART1 and USART3 clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // Enable USART1 clock on APB2 bus
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // Enable USART3 clock on APB1 bus
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); // Enable USART3 clock on APB1 bus

    // Enable GPIOA and GPIOB clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // Configure USART1 TX pin (PA9)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure USART1 RX pin (PA10)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure USART2 TX pin
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure USART2 RX pin 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure USART3 TX pin (PB10)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Configure USART3 RX pin (PB11)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Initialize USART1
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStructure);

    // Initialize USART2
	USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &USART_InitStructure);
	
    // Initialize USART3
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART3, &USART_InitStructure);

    // Enable USART1 and USART3 receive interrupts
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
    // Configure NVIC for USART1
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    // Configure NVIC for USART3
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
	
	// Configure NVIC for USART2
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStructure);

    // Enable USART1 and USART3
    USART_Cmd(USART1, ENABLE);
    USART_Cmd(USART3, ENABLE);
	USART_Cmd(USART2, ENABLE);
}

//串口1的函数
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}
//HWT101
void Serial_SendArray_HWT101(uint8_t *Array,uint16_t Length)
{
	 for (uint16_t i = 0; i < Length; i++) 
	 {
        Serial_SendByte(Array[i]);
	 }
		Serial_SendByte(0x0D); 
		Serial_SendByte(0x0A); 
		Delay_ms(100);
}
void Serial2_SendByte(uint8_t Byte)
{
	USART_SendData(USART2, Byte);
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}
void Serial2_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial2_SendByte(Array[i]);
	}
}
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}
void Serial2_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial2_SendByte(String[i]);
	}
}
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}



void Serial_SendPacket(void)
{
	Serial_SendByte(0xFF);
	Serial_SendArray(Serial1_TxPacket, 4);
	Serial_SendByte(0xFE);
}

uint8_t Serial_GetRxFlag(void)
{
	if (Serial1_RxFlag == 1)
	{
		Serial1_RxFlag = 0;
		return 1;
	}
	return 0;
}


//串口3的函数
void Serial3_SendByte(uint8_t Byte)
{
	USART_SendData(USART3, Byte);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

void Serial3_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial3_SendByte(Array[i]);
	}
}

void Serial3_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial3_SendByte(String[i]);
	}
}

void Serial3_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial3_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void Serial3_SendPacket(void)
{
	Serial3_SendByte(0xFF);
	Serial3_SendArray(Serial3_TxPacket, 4);
	Serial3_SendByte(0xFE);
}

uint8_t Serial3_GetRxFlag(void)
{
	if (Serial3_RxFlag == 1)
	{
		Serial3_RxFlag = 0;
		return 1;
	}
	return 0;
}

/*
*函数名：重定向函数
*功能：可将printf打印的位置从电脑屏幕转换至串口助手
*使用：Serial.h中写入STDIO.H，即可在主函数中直接使用printf()
**/
int fputc(int ch, FILE *f)
{
	Serial3_SendByte(ch);
	return ch;
}

/*
 *函数名：串口封装printf函数（可变参数格式）
 **/
void Serial3_Printf(char *format, ...)
{
	char String[100];
	va_list arg;//定义一个参数列表变量，前者为类型名，后者为变量名
	va_start(arg, format);//从format开始接收参数表，放在arg里面
	vsprintf(String, format, arg);//打印位置：String 格式化位置：format 参数表：arg
	va_end(arg);
	Serial3_SendString(String);//把String发送出去
}

void USART3_IRQHandler(void)
{
	static uint8_t RxState_circle = 0;		//定义表示当前状态机状态的静态变量
	static uint8_t pRxPacket_circle = 0;	//定义表示当前接收数据位置的静态变量
	static uint8_t RxState_qrcode = 0;		//定义表示当前状态机状态的静态变量
	static uint8_t pRxPacket_qrcode = 0;	//定义表示当前接收数据位置的静态变量
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)		//判断是否是USART2的接收事件触发的中断
	{
		uint8_t RxData = USART_ReceiveData(USART3);				//读取数据寄存器，存放在接收的数据变量
		
		/*使用状态机的思路，依次处理数据包的不同部分*/
		/*当前状态为0，接收数据包包头*/
		if (RxState_circle == 0)
		{
			if (RxData == 0xFF)			//如果数据确实是包头
			{
				RxState_circle = 1;			//置下一个状态
				pRxPacket_circle = 0;			//数据包的位置归零
			}
		}
		/*当前状态为1，接收数据包数据*/
		else if (RxState_circle == 1)
		{
			Serial3_RxPacket[pRxPacket_circle] = RxData;	//将数据存入数据包数组的指定位置
			pRxPacket_circle ++;				//数据包的位置自增
			if (pRxPacket_circle >= 8)			//如果收够8个数据
			{
				RxState_circle = 2;			//置下一个状态
			}
		}
		/*当前状态为2，接收数据包包尾*/
		else if (RxState_circle == 2)
		{
			if (RxData == 0xFE)			//如果数据确实是包尾部
			{
				int8_t  i;
				sum_circle=0;
				for(i=0;i<6;i++)//计算校验和
				{
					sum_circle+=Serial3_RxPacket[i];
				}
				if(sum_circle==(Serial3_RxPacket[6]<<8)+Serial3_RxPacket[7])//校验成功
				{
					sum_circle=0;
					//RxState_round = 0;			//状态归0
					//校验成功开始处理数据
					circle_x=(Serial3_RxPacket[0]<<8)+Serial3_RxPacket[1]-320;
					circle_y=(Serial3_RxPacket[2]<<8)+Serial3_RxPacket[3]-240;
					r_=(Serial3_RxPacket[4]<<8)+Serial3_RxPacket[5];
				}
			}
			RxState_circle = 0;			//状态归0
		}
		
		//接收二维码
		if (RxState_qrcode == 0)
		{
			if (RxData == 0xFA)			//如果数据确实是包头
			{
				RxState_qrcode = 1;			//置下一个状态
				pRxPacket_qrcode = 0;			//数据包的位置归零
			}
		}
		/*当前状态为1，接收数据包数据*/
		else if (RxState_qrcode == 1)
		{
			Serial3_RxPacket[pRxPacket_qrcode] = RxData;	//将数据存入数据包数组的指定位置
			pRxPacket_qrcode ++;				//数据包的位置自增
			if (pRxPacket_qrcode >= 1)			//如果收够1个数据
			{
				RxState_qrcode = 2;			//置下一个状态
			}
		}
		/*当前状态为2，接收数据包包尾*/
		else if (RxState_qrcode == 2)
		{
			if (RxData == 0xAF)			//如果数据确实是包尾部
			{
				qrcode=Serial3_RxPacket[0];
			}
			RxState_qrcode = 0;			//状态归0
		}
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);		//清除标志位
	}
}

