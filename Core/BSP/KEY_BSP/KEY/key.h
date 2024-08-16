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

void KeyInfo_Init(void);																							//KEY������ʼ��
void task_listen_key(void);																						//KEY��������
void task_run_key(void);																							//KEYִ������
unsigned char Add_Change_Key(Key_Num keyNum,Key_info p);							//KEY������Ӻ���
unsigned char Del_Key(Key_Num keyNum);																//KEY����ɾ������
unsigned char Add_Change_KeyGP(KeyGroup_Num GpNum, KeyGP_info p);			//KEY�����Ӻ���
unsigned char Del_KeyGP(KeyGroup_Num GpNum);													//KEY���ɾ������

#endif
