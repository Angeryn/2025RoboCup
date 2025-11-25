
#include "gpio.h"

void gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure={0};
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	//开漏输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;	//开漏输出
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
}



uint8_t Color_detect(uint8_t *RGB)
{
	if((RGB[0]>240&&RGB[0]<=255) && (RGB[1]>80&&RGB[1]<160) && (RGB[2]>90&&RGB[2]<160))//填入红色阈值
	{
		return 0;//红色/
	}
	else if((RGB[0]>100&&RGB[0]<160) && (RGB[1]>240&&RGB[1]<=255) && (RGB[2]>170&&RGB[2]<240))//填入绿色阈值
	{
		return 1;//绿色/
	}
	else if(RGB[0]>250 && RGB[1]>250 && RGB[2]>250)//填入白色阈值
	{ 
		return 2;//白色/
	}
	else if((RGB[0]>26&&RGB[0]<56) && (RGB[1]>36&&RGB[1]<76) && (RGB[2]>24&&RGB[2]<64))//&&(HSL[0]>80&&HSL[0]<150)&&(HSL[1]>20&&HSL[1]<80))//填入黑色阈值,黑色需要额外加入几个判断方法区别远距离的白色
	{
		return 3;//黑色
	}
	else if((RGB[0]>100&&RGB[0]<250) && (RGB[1]>250&&RGB[1]<=255) && (RGB[2]>250 && RGB[2]<=255))//填入蓝色阈值
	{
		return 4;//蓝色
	}
	else
	{
		return 5;//没有识别到颜色
	}
}
