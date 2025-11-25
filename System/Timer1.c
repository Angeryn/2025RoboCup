#include "stm32f10x.h"                  // Device header
#include "encoder.h"
#include "PID.h"
#include "Serial.h"



// 定时器初始化函数
void Timer1_Init(void)
{
    // 使能 TIM1 时钟，TIM1 挂载在 APB2 总线上
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    // 配置 TIM1 使用内部时钟
    TIM_InternalClockConfig(TIM1);

    // 配置 TIM1 定时器参数
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 1000 - 1;//修改采集编码器和进行PID修正的周期，1000的时候是10ms更新一次
    TIM_TimeBaseInitStructure.TIM_Prescaler = 720 - 1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    // 清除 TIM1 更新标志
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);

    // 使能 TIM1 更新中断
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    // 配置 TIM1 中断
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; // TIM1 更新中断对应的 IRQn
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStructure);

    // 使能 TIM1
    TIM_Cmd(TIM1, ENABLE);
    // 使能 TIM1 主输出功能（高级定时器需要）
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}



