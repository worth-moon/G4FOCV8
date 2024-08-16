#ifndef __KEY_H
#define __KEY_H
#include "key_app.h"

#undef NULL
#define NULL 0             

typedef struct
{
	Key_Num KeyNum;
	KeyGroup_Num KeyGpNum;
	unsigned char KeyStatus;
}KeyValue_Info;

void KeyInfo_Init(void);																							//KEY参数初始化
void task_listen_key(void);																						//KEY监听任务
void task_run_key(void);																							//KEY执行任务
unsigned char Add_Change_Key(Key_Num keyNum,Key_info p);							//KEY单键添加函数
unsigned char Del_Key(Key_Num keyNum);																//KEY单键删除函数
unsigned char Add_Change_KeyGP(KeyGroup_Num GpNum, KeyGP_info p);			//KEY组合添加函数
unsigned char Del_KeyGP(KeyGroup_Num GpNum);													//KEY组合删除函数

#endif
