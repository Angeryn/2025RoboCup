#include "stm32f10x.h"
#include "JY62.h"
#include "Serial.h"
#include "OLED.h"

static uint8_t RxBuffer[11];/*接收数据数组*/
static volatile uint8_t RxState = 0;/*接收状态标志位*/
static uint8_t RxIndex = 0;/*接受数组索引*/
float Yaw;
float Wx,Wy,Wz;
	
void USART1_IRQHandler(void)
{
	
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
	
		jy62_Receive_Data(RxData);
		
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		
	}
	
	
}

void HWT_Init()
{
	uint8_t unlock_register[] = {0xFF, 0xAA, 0x69, 0x88, 0xB5};
	uint8_t reset_z_axis[] = {0xFF, 0xAA, 0x76, 0x00, 0x00};
	uint8_t set_output_200Hz[] = {0xFF, 0xAA, 0x03, 0x0B, 0x00};
	uint8_t set_baudrate_115200[] = {0xFF, 0xAA, 0x04, 0x06, 0x00};
	uint8_t save_settings[] = {0xFF, 0xAA, 0x00, 0x00, 0x00};
	uint8_t restart_device[] = {0xFF, 0xAA, 0x00, 0xFF, 0x00};    
	Serial_Init();
	Serial_SendArray_HWT101(unlock_register, sizeof(unlock_register));
    Serial_SendArray_HWT101(set_output_200Hz, sizeof(set_output_200Hz));
    Serial_SendArray_HWT101(set_baudrate_115200, sizeof(set_baudrate_115200));
    Serial_SendArray_HWT101(save_settings, sizeof(save_settings));
    Serial_SendArray_HWT101(restart_device, sizeof(restart_device));
	Serial_SendArray_HWT101(reset_z_axis, sizeof(reset_z_axis));
                        
}


void jy62_Receive_Data(uint8_t RxData)
{
	
	
	uint8_t i,sum=0;
	if(RxState==0)
	{
		
		if(RxData == 0x55)
		{
			RxBuffer[RxIndex] = RxData;
			RxState = 1;
			RxIndex = 1; //进入下一状态
		}
	}
	else if(RxState==1)
	{
		
		if (RxData == 0x53)	/*判断数据内容，修改这里可以改变要读的数据内容，0x53为角度输出*/
		{
			
			RxBuffer[RxIndex] = RxData;
			RxState = 2;
			RxIndex = 2; //进入下一状态
		}
	}
	else if(RxState==2)
	{
		
		RxBuffer[RxIndex++] = RxData;
		if(RxIndex == 11)	//接收完成
		{
			for(i=0;i<10;i++)
			{
				sum = sum + RxBuffer[i]; //计算校验和
			}
			if(sum == RxBuffer[10])		//校验成功
			{
				

				/*计算数据，根据数据内容选择对应的计算公式，如果都换成uint16_t就会变成0~360°*/
//				Roll = ((int16_t) ((int16_t) RxBuffer[3] << 8 | (int16_t) RxBuffer[2])) / 32768.0f * 180.0f;
//				Pitch = ((int16_t) ((int16_t) RxBuffer[5] << 8 | (int16_t) RxBuffer[4])) / 32768.0f * 180.0f;
				Yaw = ((int16_t) ((int16_t) RxBuffer[7] << 8 | (int16_t) RxBuffer[6])) / 32768.0f * 180.0f;
			}
			RxState = 0;
			RxIndex = 0; //读取完成，回到最初状态，等待包头
		}
	}

}


