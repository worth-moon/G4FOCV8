#ifndef __RINGBUFF_H
#define __RINGBUFF_H

#include "main.h"
#include "string.h"

#define RINGSTR_MAX_CNT			30		//最多几组
#define RINGSTR_MAX_LEN			30		//每组最大长度 --MAX 255

typedef struct
{
		uint16_t Head;
		uint16_t Tail;
		uint8_t	RingStrPending[RINGSTR_MAX_CNT];
		uint8_t  RingStr[RINGSTR_MAX_CNT][RINGSTR_MAX_LEN + 1];  //数据长度+数据
}RingStr_t;

void RingStr_Init(RingStr_t *p);
uint8_t Write_RingStr(RingStr_t *p,uint8_t *pdata,uint8_t len);
uint8_t Read_RingStr(RingStr_t *p,uint8_t *pdata,uint8_t *len);
uint8_t Read_RingStrEx(RingStr_t *p,uint8_t *pdata,uint8_t *len);
void ReclaimStr(RingStr_t *p);

#endif
