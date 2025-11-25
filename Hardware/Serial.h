#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>
#include "stm32f10x.h"                  // Device header

//´®¿Ú1
void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial2_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial2_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial2_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_SendPacket(void);
uint8_t Serial_GetRxFlag(void);
void Serial_SendArray_HWT101(uint8_t *Array,uint16_t Length);
//´®¿Ú3
void Serial3_SendByte(uint8_t Byte);
void Serial3_SendArray(uint8_t *Array, uint16_t Length);
void Serial3_SendString(char *String);
void Serial3_SendNumber(uint32_t Number, uint8_t Length);
void Serial3_SendPacket(void);
uint8_t Serial3_GetRxFlag(void);

int fputc(int ch, FILE *f);
void Serial3_Printf(char *format, ...);
extern uint16_t qrcode;
extern int16_t circle_x,circle_y,r_;
#endif
