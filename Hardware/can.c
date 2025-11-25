#include "can.h"

/**********************************************************
***	ZDT_X57步进闭环控制例程
***	编写作者：ZHANGDATOU
***	技术支持：张大头闭环伺服
***	淘宝店铺：https://zhangdatou.taobao.com
***	CSDN博客：http s://blog.csdn.net/zhangdatou666
***	qq交流群：262438510
**********************************************************/

__IO CAN_t can = {0};

/**
	* @brief   CAN1_RX0接收中断
	* @param   无
	* @retval  无
	*/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	// 接收一包数据
	CAN_Receive(CAN1, CAN_FIFO0, (CanRxMsg *)(&can.CAN_RxMsg));
	
	// 一帧数据接收完成，置位帧标志位
	can.rxFrameFlag = true;
}

/**
	* @brief   CAN发送多个字节
	* @param   无
	* @retval  无
	*/
void can_SendCmd(__IO uint8_t *cmd, uint8_t len)
{
	__IO uint8_t i = 0, j = 0, k = 0, l = 0, packNum = 0;

	// 除去ID地址和功能码后的数据长度
	j = len - 2;

	// 发送数据
	while(i < j)
	{
		// 数据个数
		k = j - i;

		// 填充缓存
		can.CAN_TxMsg.StdId = 0x00;
		can.CAN_TxMsg.ExtId = ((uint32_t)cmd[0] << 8) | (uint32_t)packNum;
		can.CAN_TxMsg.Data[0] = cmd[1];
		can.CAN_TxMsg.IDE = CAN_Id_Extended;
		can.CAN_TxMsg.RTR = CAN_RTR_Data;

		// 小于8字节命令
		if(k < 8)
		{
			for(l=0; l < k; l++,i++) { can.CAN_TxMsg.Data[l + 1] = cmd[i + 2]; } can.CAN_TxMsg.DLC = k + 1;
		}
		// 大于8字节命令，分包发送，每包数据最多发送8个字节
		else
		{
			for(l=0; l < 7; l++,i++) { can.CAN_TxMsg.Data[l + 1] = cmd[i + 2]; } can.CAN_TxMsg.DLC = 8;
		}

		// 发送数据
		CAN_Transmit(CAN1, (CanTxMsg *)(&can.CAN_TxMsg));

		// 记录发送的第几包的数据
		++packNum;
	}
}
