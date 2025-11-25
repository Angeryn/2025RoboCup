#include "stm32f10x.h"                  // Device header

// 定义四个编码器的计数值和采样值
int16_t Encoder_Count_A;
int16_t Encoder_Count_B;
int16_t Encoder_Count_C;
int16_t Encoder_Count_D;

//上一次编码器传入的脉冲值
int16_t Encoder_Count_A0;
int16_t Encoder_Count_B0;
int16_t Encoder_Count_C0;
int16_t Encoder_Count_D0;

int16_t Val_A;
int16_t Val_B;
int16_t Val_C;
int16_t Val_D;

// 编码器初始化函数
void Encoder_Init(void)
{
    // 使能 GPIOA、GPIOB 和 AFIO 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	// 禁用 JTAG/SWD 调试接口，释放 PB3 引脚
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
    // 配置 GPIO 为上拉输入模式
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    // 配置编码器 A 的引脚 PB12 和 PB13
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置编码器 B 的引脚 PB14 和 PB15
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置编码器 C 的引脚 PA15 和 PB3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置编码器 D 的引脚 PA11 和 PA8
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_8;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 连接外部中断线到相应的 GPIO 引脚，这些是用于检测下降沿的
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource3);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);

    // 配置外部中断
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line12 | EXTI_Line14 | EXTI_Line3 | EXTI_Line8 ;//只有A相的下降沿才触发中断，这四个都是A相
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
    EXTI_Init(&EXTI_InitStructure);

    // 配置 NVIC 中断
    NVIC_InitTypeDef NVIC_InitStructure;

    // 编码器 A 和 B 的中断配置
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    // 编码器 C 的中断配置
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    // 编码器 D 的中断配置
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStructure);
}


// EXTI15_10 中断处理函数（编码器 A 和 B）
void EXTI15_10_IRQHandler(void)
{
    __disable_irq();
	//A轮
    if (EXTI_GetITStatus(EXTI_Line12) == SET)//A相下降沿触发中断
    {
		
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == 0)//B相低电平，为左侧轮正转
        {
            Encoder_Count_A++;
        }
		else//B相低电平，为反转
        {
            Encoder_Count_A--;
        }
        EXTI_ClearITPendingBit(EXTI_Line12);//清除标志位
    }
	//B轮
    else if (EXTI_GetITStatus(EXTI_Line14) == SET)//A相下降沿触发中断
    {
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) == 0)//B相低电平，为左侧轮正转
        {
            Encoder_Count_B++;
        }
        else//B相低电平，为反转
        {
            Encoder_Count_B--;
        }
        EXTI_ClearITPendingBit(EXTI_Line14);//清除标志位
    }
    __enable_irq();
}

// EXTI3 中断处理函数（编码器 C）
void EXTI3_IRQHandler(void)//C轮是B相下降沿进中断判断A相，继续用A相下降沿判断B相的话要改前面许多东西
{
    __disable_irq();
    if (EXTI_GetITStatus(EXTI_Line3) == SET)//B相下降沿触发中断
    {
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0)//A相低电平，为逆时针旋转
        {
            Encoder_Count_C++;
        }
        else
        {
            Encoder_Count_C--;
        }
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
    __enable_irq();
}

// EXTI9_5 中断处理函数（编码器 D）
void EXTI9_5_IRQHandler(void)
{
    __disable_irq();
    if (EXTI_GetITStatus(EXTI_Line8) == SET)//B相下降沿进中断
    {
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11) == 0)//A相低电平，为逆时针旋转
        {
            Encoder_Count_D++;
        }
        else
        {
            Encoder_Count_D--;
        }
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
    __enable_irq();
}

/*编码器读当前位置*/
int16_t Encoder_A_Location(void)
{
    __disable_irq();
    int16_t Temp = Encoder_Count_A;
    __enable_irq();
    return Temp;
}

int16_t Encoder_B_Location(void)
{
    __disable_irq();
    int16_t Temp = Encoder_Count_B;
    __enable_irq();
    return Temp;
}

int16_t Encoder_C_Location(void)
{
    __disable_irq();
    int16_t Temp = Encoder_Count_C;
    __enable_irq();
    return Temp;
}

int16_t Encoder_D_Location(void)
{
    __disable_irq();
    int16_t Temp = Encoder_Count_D;
    __enable_irq();
    return Temp;
}

/*编码器读当前速度*/
int16_t Encoder_A_Val(void)
{
    __disable_irq();
    Val_A = (int16_t)((uint16_t)Encoder_Count_A-(uint16_t)Encoder_Count_A0);
	Encoder_Count_A0=Encoder_Count_A;
    __enable_irq();
    return Val_A;
}

int16_t Encoder_B_Val(void)
{
    __disable_irq();
    Val_B = (int16_t)((uint16_t)Encoder_Count_B-(uint16_t)Encoder_Count_B0);
	Encoder_Count_B0=Encoder_Count_B;
    __enable_irq();
    return Val_B;
}

int16_t Encoder_C_Val(void)
{
    __disable_irq();
    Val_C = (int16_t)((uint16_t)Encoder_Count_C-(uint16_t)Encoder_Count_C0);
	Encoder_Count_C0=Encoder_Count_C;
    __enable_irq();
    return Val_C;
}

int16_t Encoder_D_Val(void)
{
    __disable_irq();
    Val_D = (int16_t)((uint16_t)Encoder_Count_D-(uint16_t)Encoder_Count_D0);
	Encoder_Count_D0=Encoder_Count_D;
    __enable_irq();
    return Val_D;
}

