#ifndef __KEY_APP_H
#define __KEY_APP_H	 


/************************************************************************
														根据实际需要修改项
************************************************************************/

#define KEYGP_MAX_KEYNUM_SET	5 								//组合按键数量Max值

#define	KEY_CHECKTIME 				30								//消抖时间

#define LP_TIME								360							//长按时间阈值
#define SP_TIME								60								//短按时间阈值

#define DEBUG_PRINTF					0									//调试打印

//添加新按键，这里新加一个key编号
typedef enum {
	Key1 = 0,
  Key2,  
  Key3,
  Key4,
  KeyCnt,
}Key_Num;

//添加新组合，这里新加一个组合编号
typedef enum {
	KeyGP1 = 0,
  KeyGP2,  
  KeyGP3,
  KeyGPCnt,
}KeyGroup_Num;


/************************************************************************
														不需要修改项
************************************************************************/
#define KEY_USE							1									//按键使用				
#define	KEY_NOUSE						0									//按键不使用
#define KEY_NOCLOCK					0									//支持连发
#define KEY_CLOCK						1									//不支持连发
#define LP									0									//长按
#define	SP									1									//短按
#define LOOP								2									//周期
#define KEY_ON							0									//按键有效状态
#define KEY_OFF							1									//按键无效状态

#define KEYGP_USE						1									//按键组合使用				
#define	KEYGP_NOUSE					0									//按键组合不使用
#define KEYGP_ON						1									//按键组合生效
#define	KEYGP_OFF						0									//按键组合没生效

#define	KEYSTRICT_ON				1									//严格检查按键组合输入顺序
#define	KEYSTRICT_OFF				0									//不严格检查按键组合输入顺序

#define KEYGPMAXKEY					8									//组合键最大支持8键组合
#define MIN(a,b)   		 			(((a) < (b)) ? (a) : (b))
#define	KEYGP_MAX_KEYNUM		MIN(KEYGPMAXKEY,KEYGP_MAX_KEYNUM_SET)

//Error表
#define ERROR_OK						0x00							//成功
#define	ERROR_NUM						0xE1							//枚举入参错误/按键数量错误
#define	ERROR_KEYGPIO				0xE2							//GPIO信息错误
#define ERROR_USEFLG				0xE3							//使用状态不符当前功能函数
#define	ERROR_GPKEYNUM			0xE4							//组合按键有误
#define	ERROR_GPKEYUSE			0xE5							//组合按键的单键功能没启用
#define ERROR_KEYUSEGP			0xE6							//按键正被组合按键使用中
#define ERROR_KEYGPMAX			0xE7							//按键组合复合次数达到最大值了
#define ERROR_KEYGPMIN			0xE8							//按键组合复合次数达到最小值了
#define ERROR_GPKEYREPET		0xE9							//组合按键有重复

/************************************************************************
														结构体定义和函数定义
************************************************************************/
typedef struct
{
	unsigned char useFlg;												//按键使用标识 --KEY_USE/KEY_NOUSE
	unsigned char mode;													//按键模式		 --KEY_NOCLOCK/KEY_CLOCK
	unsigned short AutoTime;										//KEY_NOCLOCK模式下有效，执行周期 -- 建议设置为消抖时间的倍数
	void (*KeyLPRun)(void);											//长按执行函数，KEY_CLOCK有效
	void (*KeySPRun)(void);											//短按执行函数，KEY_CLOCK有效
	void (*KeyLOOPRun)(void);										//周期执行函数，KEY_NOCLOCK有效	
}Key_info;


typedef struct
{
	unsigned char useFlg;													//组合按键使用标识 --KEYGP_USE/KEYGP_NOUSE
	unsigned char GPnum;													//组合按键数量
	unsigned char KeyNum[KEYGP_MAX_KEYNUM];				//组合按键Key_num集合
	unsigned char	GetKeyNumStrict;								//是否严格按照Key_num集合获取键值组合，即先获取数据1，在获取数据2才能触发。KEYSTRICT_ON/KEYSTRICT_OFF
	void (*KeyGPRun)(void);												//组合完成执行函数
}KeyGP_info;

void Key_Init(void);
unsigned char GetKeyValue(Key_Num num);
void Add_Key_demo(void);
void Add_KeyGP_demo(void);

void handleButtonState(void);
#endif
