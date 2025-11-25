#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Key_Init(void)
{
	// 使能 GPIOA、GPIOB 和 GPIOC 的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

	// 禁用 JTAG - SWD 复用功能，释放 PB4 引脚
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
    GPIO_InitTypeDef GPIO_InitStructure;

    // 配置 PA0
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
    // 配置 PC4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 配置 PC13 和 PC14
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/*获取键码*/
uint8_t Key_GetNum(void)
{
    uint8_t KeyNum = 0;

    // 检测 PA0 按键
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
    {
        Delay_ms(20);  // 消抖延时
        while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0);  // 等待按键释放
        Delay_ms(20);  // 消抖延时
        KeyNum = 1;
    }

    // 检测 PB4 按键
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0)
    {
        Delay_ms(20);  // 消抖延时
        while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == 0);  // 等待按键释放
        Delay_ms(20);  // 消抖延时
        KeyNum = 2;
    }

    // 检测 PC13 按键
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == 0)
    {
        Delay_ms(20);  // 消抖延时
        while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == 0);  // 等待按键释放
        Delay_ms(20);  // 消抖延时
        KeyNum = 3;
    }

    // 检测 PC14 按键
    if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) == 0)
    {
        Delay_ms(20);  // 消抖延时
        while (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14) == 0);  // 等待按键释放
        Delay_ms(20);  // 消抖延时
        KeyNum = 4;
    }

    return KeyNum;
}
