#ifndef __KEY_APP_H
#define __KEY_APP_H	 


/************************************************************************
														����ʵ����Ҫ�޸���
************************************************************************/

#define KEYGP_MAX_KEYNUM_SET	5 								//��ϰ�������Maxֵ

#define	KEY_CHECKTIME 				30								//����ʱ��

#define LP_TIME								360							//����ʱ����ֵ
#define SP_TIME								60								//�̰�ʱ����ֵ

#define DEBUG_PRINTF					0									//���Դ�ӡ

//����°����������¼�һ��key���
typedef enum {
	Key1 = 0,
  Key2,  
  Key3,
  Key4,
  KeyCnt,
}Key_Num;

//�������ϣ������¼�һ����ϱ��
typedef enum {
	KeyGP1 = 0,
  KeyGP2,  
  KeyGP3,
  KeyGPCnt,
}KeyGroup_Num;


/************************************************************************
														����Ҫ�޸���
************************************************************************/
#define KEY_USE							1									//����ʹ��				
#define	KEY_NOUSE						0									//������ʹ��
#define KEY_NOCLOCK					0									//֧������
#define KEY_CLOCK						1									//��֧������
#define LP									0									//����
#define	SP									1									//�̰�
#define LOOP								2									//����
#define KEY_ON							0									//������Ч״̬
#define KEY_OFF							1									//������Ч״̬

#define KEYGP_USE						1									//�������ʹ��				
#define	KEYGP_NOUSE					0									//������ϲ�ʹ��
#define KEYGP_ON						1									//���������Ч
#define	KEYGP_OFF						0									//�������û��Ч

#define	KEYSTRICT_ON				1									//�ϸ��鰴���������˳��
#define	KEYSTRICT_OFF				0									//���ϸ��鰴���������˳��

#define KEYGPMAXKEY					8									//��ϼ����֧��8�����
#define MIN(a,b)   		 			(((a) < (b)) ? (a) : (b))
#define	KEYGP_MAX_KEYNUM		MIN(KEYGPMAXKEY,KEYGP_MAX_KEYNUM_SET)

//Error��
#define ERROR_OK						0x00							//�ɹ�
#define	ERROR_NUM						0xE1							//ö����δ���/������������
#define	ERROR_KEYGPIO				0xE2							//GPIO��Ϣ����
#define ERROR_USEFLG				0xE3							//ʹ��״̬������ǰ���ܺ���
#define	ERROR_GPKEYNUM			0xE4							//��ϰ�������
#define	ERROR_GPKEYUSE			0xE5							//��ϰ����ĵ�������û����
#define ERROR_KEYUSEGP			0xE6							//����������ϰ���ʹ����
#define ERROR_KEYGPMAX			0xE7							//������ϸ��ϴ����ﵽ���ֵ��
#define ERROR_KEYGPMIN			0xE8							//������ϸ��ϴ����ﵽ��Сֵ��
#define ERROR_GPKEYREPET		0xE9							//��ϰ������ظ�

/************************************************************************
														�ṹ�嶨��ͺ�������
************************************************************************/
typedef struct
{
	unsigned char useFlg;												//����ʹ�ñ�ʶ --KEY_USE/KEY_NOUSE
	unsigned char mode;													//����ģʽ		 --KEY_NOCLOCK/KEY_CLOCK
	unsigned short AutoTime;										//KEY_NOCLOCKģʽ����Ч��ִ������ -- ��������Ϊ����ʱ��ı���
	void (*KeyLPRun)(void);											//����ִ�к�����KEY_CLOCK��Ч
	void (*KeySPRun)(void);											//�̰�ִ�к�����KEY_CLOCK��Ч
	void (*KeyLOOPRun)(void);										//����ִ�к�����KEY_NOCLOCK��Ч	
}Key_info;


typedef struct
{
	unsigned char useFlg;													//��ϰ���ʹ�ñ�ʶ --KEYGP_USE/KEYGP_NOUSE
	unsigned char GPnum;													//��ϰ�������
	unsigned char KeyNum[KEYGP_MAX_KEYNUM];				//��ϰ���Key_num����
	unsigned char	GetKeyNumStrict;								//�Ƿ��ϸ���Key_num���ϻ�ȡ��ֵ��ϣ����Ȼ�ȡ����1���ڻ�ȡ����2���ܴ�����KEYSTRICT_ON/KEYSTRICT_OFF
	void (*KeyGPRun)(void);												//������ִ�к���
}KeyGP_info;

void Key_Init(void);
unsigned char GetKeyValue(Key_Num num);
void Add_Key_demo(void);
void Add_KeyGP_demo(void);

void handleButtonState(void);
#endif
