#include "key.h"
#include "ringbuff.h"

static unsigned char  Timecnt[KeyCnt];										//��������
static unsigned char  Get_keyValue[KeyCnt];								//����ʵʱ״̬
static unsigned char  Get_keyValueX[KeyCnt];							//����ǰһ����״̬
static unsigned short Get_keyChooseTimeCnt[KeyCnt];				//����ʱ�����
static unsigned char  KeyGPFlag[KeyCnt];									//�Ƿ���Ϊ��ϼ���ʶ
static Key_info KeyInfo[KeyCnt];													//����������Ϣ

static KeyGP_info	KeyGpInfo[KeyGPCnt];										//��ϰ�����Ϣ
static unsigned char  Get_keyGPStatus[KeyGPCnt];					//���ʵʱ״̬
static unsigned char  Get_keyGPStatusX[KeyGPCnt];					//���ǰһ����״̬
static unsigned char	KeyGPStatusValueNow[KeyGPCnt];			//���ʵʱ����ֵ
static unsigned char	KeyGPStatusValueFinish[KeyGPCnt];		//�����ɲο�����ֵ

static RingStr_t	Key_Value;															//��ȷ��������ϵĻ��ζ���

void KeyInfo_Init()
{
	unsigned char i;
	
	//�����
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
	
	//���
	for(i = 0;i < KeyGPCnt;i++)
	{
		Get_keyGPStatus[i] = 0xFF;
		Get_keyGPStatusX[i] = 0xFF;
		KeyGPStatusValueNow[i] = 0;
		KeyGpInfo[i].useFlg = KEYGP_NOUSE;
		KeyGpInfo[i].KeyGPRun = NULL;		
	}
	
	//���һ�����ζ���
	RingStr_Init(&Key_Value);
}

//�����������ܺ���
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
	
	//��ɾ�����������ʹ�ã�����ɾ��
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
			//���״̬��ֻ��Ҫʵʱ״̬
			if(KeyGPFlag[keyNum] != 0)
			{
					Get_keyValue[keyNum] = KEY_ON;
					return;
			}	
			
			if(Get_keyValueX[keyNum] == KEY_OFF && mode == KEY_CLOCK)
			{		
				Get_keyValue[keyNum] = KEY_ON;
#if DEBUG_PRINTF
				printf("����key%d����\r\n",(unsigned char)keyNum+1);
#endif	
			}
			else if(mode == KEY_NOCLOCK && Get_keyChooseTimeCnt[keyNum] == 0)
			{
				Get_keyValue[keyNum] = KEY_ON;
#if DEBUG_PRINTF
				printf("���ڰ���key%d����\r\n",(unsigned char)keyNum+1);
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

//��ϰ������ܺ���
static void Get_GPFinishStatusValue(unsigned char KeyGPNum,unsigned char * OutData)
{
	unsigned char tmp = 0,i = 0;
	
	for(i = 0;i < KeyGPNum;i++)
	{
		tmp |= 1 << i;
	}
	
	*OutData = tmp;
}

//�����ϰ���Ԫ���ظ���
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
	
	//������ϱ�ʶ��ȡ����������
	if(KeyGpInfo[GpNum].useFlg == KEYGP_NOUSE)
	{
		for(i = 0;i < p.GPnum;i++)
		{
			if(KeyGPFlag[p.KeyNum[i]] == 0xFF)
				return ERROR_KEYGPMAX;
			//�����ظ���ֵ�����
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
	
	//�ָ���������
	for(i = 0;i < KeyGpInfo[GpNum].GPnum;i++)
	{
		if(KeyGPFlag[KeyGpInfo[GpNum].KeyNum[i]] == 0)
				return ERROR_KEYGPMIN;
		//�����ظ���ֵ�����
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
	
	//tmpΪ1���ֵ�λ�� ��û��1��Ϊ0��
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
	
	//���tmp�±�֮��İ����Ƿ��£����������á�
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
//�������� --����1msѭ������ִ��
void task_listen_key(void)
{
	unsigned char i;
	KeyValue_Info KeyValueInfo;
	
	//�������
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
				printf("������ģʽ��key%d�������ɿ�,����ʱ��Ϊ%dMs\r\n",(unsigned char)i+1,Get_keyChooseTimeCnt[i]);
#endif
				//�̰�
				if((Get_keyChooseTimeCnt[i] != 0) && (Get_keyChooseTimeCnt[i] < LP_TIME) && (Get_keyChooseTimeCnt[i] >= SP_TIME))
				{
					KeyValueInfo.KeyNum = (Key_Num)i;
					KeyValueInfo.KeyGpNum = (KeyGroup_Num)KeyGPCnt;
					KeyValueInfo.KeyStatus = SP;
					Write_RingStr(&Key_Value,(unsigned char *)&KeyValueInfo,sizeof(KeyValueInfo));

				}		
				//����
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
				printf("����ģʽ��key%d��������ʱ��Ϊ%dMs �Զ�����ʱ�䣺%dMs\r\n",(unsigned char)i+1,Get_keyChooseTimeCnt[i],KeyInfo[i].AutoTime);
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
	
	//��ϼ��
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
				printf("��� %d�Ѵ���\r\n",(unsigned char)i+1);
#endif
				KeyValueInfo.KeyNum = (Key_Num)KeyCnt;
				KeyValueInfo.KeyGpNum = (KeyGroup_Num)i;
				KeyValueInfo.KeyStatus = 0xFF;
				Write_RingStr(&Key_Value,(unsigned char *)&KeyValueInfo,sizeof(KeyValue_Info));

				//���һ����ϣ����ü�ֵ��
				KeyGPStatusValueNow[i] = 0;

		}
		Get_keyGPStatusX[i] = Get_keyGPStatus[i];		
	}
}
//ִ�к��� --������ѭ����ִ��
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

