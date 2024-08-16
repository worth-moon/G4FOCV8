#include "key.h"
#include "ringbuff.h"

static unsigned char  Timecnt[KeyCnt];										//消抖计数
static unsigned char  Get_keyValue[KeyCnt];								//键入实时状态
static unsigned char  Get_keyValueX[KeyCnt];							//键入前一周期状态
static unsigned short Get_keyChooseTimeCnt[KeyCnt];				//键入时间计数
static unsigned char  KeyGPFlag[KeyCnt];									//是否作为组合键标识
static Key_info KeyInfo[KeyCnt];													//单键按键信息

static KeyGP_info	KeyGpInfo[KeyGPCnt];										//组合按键信息
static unsigned char  Get_keyGPStatus[KeyGPCnt];					//组合实时状态
static unsigned char  Get_keyGPStatusX[KeyGPCnt];					//组合前一周期状态
static unsigned char	KeyGPStatusValueNow[KeyGPCnt];			//组合实时键入值
static unsigned char	KeyGPStatusValueFinish[KeyGPCnt];		//组合完成参考键入值

static RingStr_t	Key_Value;															//已确定按键组合的环形队列

void KeyInfo_Init()
{
	unsigned char i;
	
	//非组合
	for(i = 0;i < KeyCnt;i++)
	{
		Timecnt[i] = 0;
		Get_keyChooseTimeCnt[i] = 0;
		Get_keyValue[i] = 0xFF;
		Get_keyValueX[i] = 0xFF;
		KeyGPFlag[i] = 0;
		KeyInfo[i].useFlg = KEY_NOUSE;
		KeyInfo[i].KeyLPRun = NULL;
		KeyInfo[i].KeySPRun = NULL;
		KeyInfo[i].KeyLOOPRun = NULL;
	}
	
	//组合
	for(i = 0;i < KeyGPCnt;i++)
	{
		Get_keyGPStatus[i] = 0xFF;
		Get_keyGPStatusX[i] = 0xFF;
		KeyGPStatusValueNow[i] = 0;
		KeyGpInfo[i].useFlg = KEYGP_NOUSE;
		KeyGpInfo[i].KeyGPRun = NULL;		
	}
	
	//添加一个环形队列
	RingStr_Init(&Key_Value);
}

//单键按键功能函数
unsigned char Add_Change_Key(Key_Num keyNum,Key_info p)
{
	if(keyNum >= KeyCnt)
		return ERROR_NUM;
	
	if(p.useFlg == KEY_NOUSE)
		return ERROR_USEFLG;
	
	KeyInfo[keyNum] = p;
	return ERROR_OK;
}

unsigned char Del_Key(Key_Num keyNum)
{
	if(keyNum >= KeyCnt)
		return ERROR_NUM;
	
	//被删按键在组合中使用，不给删除
	if(KeyGPFlag[keyNum] != 0)
		return ERROR_KEYUSEGP;
	
	Get_keyValue[keyNum] = 0xFF;
	Get_keyValueX[keyNum] = 0xFF;
	KeyInfo[keyNum].useFlg = KEY_NOUSE;

	return ERROR_OK;
}

static void CheckKey(Key_Num keyNum,unsigned char mode)
{
	if(keyNum >= KeyCnt) 
		return;
	
	Timecnt[keyNum]++;
	if(Timecnt[keyNum] >= KEY_CHECKTIME)
	{
		Timecnt[keyNum] = 0;
		if(GetKeyValue(keyNum) == KEY_ON)
		{	
			//组合状态下只需要实时状态
			if(KeyGPFlag[keyNum] != 0)
			{
					Get_keyValue[keyNum] = KEY_ON;
					return;
			}	
			
			if(Get_keyValueX[keyNum] == KEY_OFF && mode == KEY_CLOCK)
			{		
				Get_keyValue[keyNum] = KEY_ON;
#if DEBUG_PRINTF
				printf("按下key%d按键\r\n",(unsigned char)keyNum+1);
#endif	
			}
			else if(mode == KEY_NOCLOCK && Get_keyChooseTimeCnt[keyNum] == 0)
			{
				Get_keyValue[keyNum] = KEY_ON;
#if DEBUG_PRINTF
				printf("周期按下key%d按键\r\n",(unsigned char)keyNum+1);
#endif					
			}
			Get_keyChooseTimeCnt[keyNum]+=KEY_CHECKTIME;
			
			if(mode == KEY_NOCLOCK && Get_keyChooseTimeCnt[keyNum] >= KeyInfo[keyNum].AutoTime)
			{
				Get_keyValue[keyNum] = KEY_OFF;		
			}
		}
		else
		{
			Get_keyValue[keyNum] = KEY_OFF;
			Timecnt[keyNum] = 0;
		}	
	}
}

//组合按键功能函数
static void Get_GPFinishStatusValue(unsigned char KeyGPNum,unsigned char * OutData)
{
	unsigned char tmp = 0,i = 0;
	
	for(i = 0;i < KeyGPNum;i++)
	{
		tmp |= 1 << i;
	}
	
	*OutData = tmp;
}

//检查组合按键元素重复性
static unsigned char CheckKeyGPNumRepet(unsigned char * InData,unsigned char DataLen)
{
	unsigned char i,j;
	for(i = 0;i<DataLen;i++)
	{
		for(j = i+1;j<DataLen;j++)
		{
			if(InData[i] == InData[j])
				return 1;
		}
	}
	
	return 0;
}

unsigned char Add_Change_KeyGP(KeyGroup_Num GpNum, KeyGP_info p)
{
	unsigned char i;
	
	if(GpNum >= KeyGPCnt || p.GPnum > KEYGP_MAX_KEYNUM || p.GPnum > KeyCnt || p.GPnum > 8|| p.GPnum < 2)
			return ERROR_NUM;	
	
	if(p.useFlg == KEYGP_NOUSE)
			return ERROR_USEFLG;
	
	if(CheckKeyGPNumRepet(p.KeyNum,p.GPnum))
			return ERROR_GPKEYREPET;
	
	for(i = 0;i < p.GPnum;i++)
	{
		if(p.KeyNum[i] >= KeyCnt)
			return ERROR_GPKEYNUM;
		if(KeyInfo[p.KeyNum[i]].useFlg == KEY_NOUSE)
			return ERROR_GPKEYUSE;
	}
	
	//赋予组合标识，取消单键作用
	if(KeyGpInfo[GpNum].useFlg == KEYGP_NOUSE)
	{
		for(i = 0;i < p.GPnum;i++)
		{
			if(KeyGPFlag[p.KeyNum[i]] == 0xFF)
				return ERROR_KEYGPMAX;
			//存在重复赋值的情况
			KeyGPFlag[p.KeyNum[i]]++;
		}		
	}

	KeyGPStatusValueNow[GpNum] = 0;
	Get_GPFinishStatusValue(p.GPnum,&KeyGPStatusValueFinish[GpNum]);
	KeyGpInfo[GpNum] = p;		
	
	return ERROR_OK;
}

unsigned char Del_KeyGP(KeyGroup_Num GpNum)
{
	unsigned char i;
	
	if(GpNum >= KeyGPCnt)
		return ERROR_NUM;
	
	if(KeyGpInfo[GpNum].useFlg == KEYGP_NOUSE)
		return ERROR_USEFLG;
	
	//恢复单键作用
	for(i = 0;i < KeyGpInfo[GpNum].GPnum;i++)
	{
		if(KeyGPFlag[KeyGpInfo[GpNum].KeyNum[i]] == 0)
				return ERROR_KEYGPMIN;
		//存在重复赋值的情况
		KeyGPFlag[KeyGpInfo[GpNum].KeyNum[i]]--;		
	}

	Get_keyGPStatus[GpNum] = 0xFF;
	Get_keyGPStatusX[GpNum] = 0xFF;
	
	KeyGpInfo[GpNum].useFlg = KEYGP_NOUSE;
	
	return ERROR_OK;
}

static void CheckKeyGP_NoStrict(KeyGroup_Num GpNum,KeyGP_info p)
{
	unsigned char i;
	
	if(GpNum >= KeyGPCnt)
		return;	
	
	for(i = 0;i < p.GPnum;i++)
	{
		if(Get_keyValue[p.KeyNum[i]] == KEY_ON)
			KeyGPStatusValueNow[GpNum] |=  1 << i;
		else
			KeyGPStatusValueNow[GpNum] &= ~(1 << i);
	}	

	if(KeyGPStatusValueNow[GpNum] == KeyGPStatusValueFinish[GpNum])
		Get_keyGPStatus[GpNum] = KEYGP_ON;
	else
		Get_keyGPStatus[GpNum] = KEYGP_OFF;	
}

static void GetCheckKeyNum(unsigned char KeyGPStatusValueNow,unsigned char *Num)
{
	unsigned char i;
	unsigned char Bit = 0x80;
	
	for(i = 0;i < 8;i++)
	{
		if(KeyGPStatusValueNow & Bit)
			break;
		
		Bit >>= 1;
	}
	
	*Num = 8 - i;
}

static void CheckKeyGP_Strict(KeyGroup_Num GpNum,KeyGP_info p)
{
	unsigned char tmp;
	unsigned char i;
	if(GpNum >= KeyGPCnt)
		return;		
	
	//tmp为1出现的位置 。没有1则为0；
	GetCheckKeyNum(KeyGPStatusValueNow[GpNum],&tmp);
	
	if(tmp > p.GPnum)
		return;
	
	for(i = 0;i <= tmp && i < p.GPnum;i++)
	{
		if(Get_keyValue[p.KeyNum[i]] == KEY_ON)
			KeyGPStatusValueNow[GpNum] |=  1 << i;
		else
			KeyGPStatusValueNow[GpNum] &= ~(1 << i);		
	}
	
	//检查tmp下标之后的按键是否按下，按下则重置。
	for(;i < p.GPnum;i++)
	{
		if(Get_keyValue[p.KeyNum[i]] == KEY_ON)
			KeyGPStatusValueNow[GpNum] = 0;
	}
	
	if(KeyGPStatusValueNow[GpNum] == KeyGPStatusValueFinish[GpNum])
		Get_keyGPStatus[GpNum] = KEYGP_ON;
	else
		Get_keyGPStatus[GpNum] = KEYGP_OFF;
}
//监听函数 --放在1ms循环里面执行
void task_listen_key(void)
{
	unsigned char i;
	KeyValue_Info KeyValueInfo;
	
	//单键检测
	for(i = 0; i < KeyCnt;i++)
	{
		if(KeyInfo[i].useFlg == KEY_USE)
		{
			CheckKey((Key_Num)i,KeyInfo[i].mode);
		}		
		if(Get_keyValueX[i] == KEY_ON && Get_keyValue[i] == KEY_OFF && KeyGPFlag[i] == 0)
		{		
			if(KeyInfo[i].mode ==	KEY_CLOCK)
			{
#if DEBUG_PRINTF
				printf("非连发模式：key%d按键已松开,键入时间为%dMs\r\n",(unsigned char)i+1,Get_keyChooseTimeCnt[i]);
#endif
				//短按
				if((Get_keyChooseTimeCnt[i] != 0) && (Get_keyChooseTimeCnt[i] < LP_TIME) && (Get_keyChooseTimeCnt[i] >= SP_TIME))
				{
					KeyValueInfo.KeyNum = (Key_Num)i;
					KeyValueInfo.KeyGpNum = (KeyGroup_Num)KeyGPCnt;
					KeyValueInfo.KeyStatus = SP;
					Write_RingStr(&Key_Value,(unsigned char *)&KeyValueInfo,sizeof(KeyValueInfo));

				}		
				//长按
				else if((Get_keyChooseTimeCnt[i] != 0) && (Get_keyChooseTimeCnt[i] >= LP_TIME))
				{
					KeyValueInfo.KeyNum = (Key_Num)i;
					KeyValueInfo.KeyGpNum = (KeyGroup_Num)KeyGPCnt;
					KeyValueInfo.KeyStatus = LP;
					Write_RingStr(&Key_Value,(unsigned char *)&KeyValueInfo,sizeof(KeyValue_Info));					
				}					
			}
			
			else if (KeyInfo[i].mode ==	KEY_NOCLOCK)
			{
#if DEBUG_PRINTF
				printf("连发模式：key%d按键键入时间为%dMs 自动连发时间：%dMs\r\n",(unsigned char)i+1,Get_keyChooseTimeCnt[i],KeyInfo[i].AutoTime);
#endif
				if(Get_keyChooseTimeCnt[i] != 0)
				{
					KeyValueInfo.KeyNum = (Key_Num)i;
					KeyValueInfo.KeyGpNum = (KeyGroup_Num)KeyGPCnt;
					KeyValueInfo.KeyStatus = LOOP;
					Write_RingStr(&Key_Value,(unsigned char *)&KeyValueInfo,sizeof(KeyValue_Info));						
				}	
			}
			
			Get_keyChooseTimeCnt[i] = 0;
		}	
		Get_keyValueX[i] = Get_keyValue[i];
	}
	
	//组合检测
	for(i = 0; i < KeyGPCnt;i++)
	{
		if(KeyGpInfo[i].useFlg == KEYGP_USE)
		{
			if(KeyGpInfo[i].GetKeyNumStrict == KEYSTRICT_OFF)
				CheckKeyGP_NoStrict((KeyGroup_Num)i,KeyGpInfo[i]);
			else if(KeyGpInfo[i].GetKeyNumStrict == KEYSTRICT_ON)
				CheckKeyGP_Strict((KeyGroup_Num)i,KeyGpInfo[i]);
		}
		if(Get_keyGPStatusX[i] == KEYGP_OFF && Get_keyGPStatus[i] == KEYGP_ON)
		{	
#if DEBUG_PRINTF
				printf("组合 %d已触发\r\n",(unsigned char)i+1);
#endif
				KeyValueInfo.KeyNum = (Key_Num)KeyCnt;
				KeyValueInfo.KeyGpNum = (KeyGroup_Num)i;
				KeyValueInfo.KeyStatus = 0xFF;
				Write_RingStr(&Key_Value,(unsigned char *)&KeyValueInfo,sizeof(KeyValue_Info));

				//完成一次组合，重置键值。
				KeyGPStatusValueNow[i] = 0;

		}
		Get_keyGPStatusX[i] = Get_keyGPStatus[i];		
	}
}
//执行函数 --放在主循环里执行
void task_run_key(void)
{
	KeyValue_Info KeyValueInfo;
	unsigned char len = 0;
	
	if(Read_RingStrEx(&Key_Value,(unsigned char *)&KeyValueInfo,&len) == 0)
	{
		if(len != sizeof(KeyValue_Info))
		{
			return;
		}
		
		if(KeyValueInfo.KeyGpNum < KeyGPCnt && KeyGpInfo[KeyValueInfo.KeyGpNum].KeyGPRun != NULL)
		{
			KeyGpInfo[KeyValueInfo.KeyGpNum].KeyGPRun();
		}
		else if(KeyValueInfo.KeyNum < KeyCnt)
		{
			if(KeyValueInfo.KeyStatus == SP && KeyInfo[KeyValueInfo.KeyNum].KeySPRun != NULL)
			{
				KeyInfo[KeyValueInfo.KeyNum].KeySPRun();
			}
			else if(KeyValueInfo.KeyStatus == LP && KeyInfo[KeyValueInfo.KeyNum].KeyLPRun != NULL)
			{
				KeyInfo[KeyValueInfo.KeyNum].KeyLPRun();
			}
			else if(KeyValueInfo.KeyStatus == LOOP && KeyInfo[KeyValueInfo.KeyNum].KeyLOOPRun != NULL)
			{
				KeyInfo[KeyValueInfo.KeyNum].KeyLOOPRun();
			}	
		}
	}
}

