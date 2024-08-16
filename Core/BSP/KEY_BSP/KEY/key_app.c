//#include "sys.h"
#include "key.h"
#include "key_app.h"
#include "main.h"
#include "gpio.h"

float count;
/****************用户修改区域 -- Start -- demo***********************/

/************************************
按键硬件初始化

---这里不仅仅可以用于stm32的管脚初始化，也可以用在
74H589的IO初始化，以及读取74H589的函数实现
************************************/
void Key_BspInit()
{
//	GPIO_InitTypeDef  GPIO_InitStructure;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_6;	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 									
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/************************************************
获取按键实时值	KEY_ON是有效，KEY_OFF是无效

--这里不仅仅可以用于stm32的管脚读取，也可以是用
74H589芯片的拓展IO读取
*************************************************/
unsigned char GetKeyValue(Key_Num num)
{
	unsigned char status = 0xFF;
	
	switch(num)
	{
		case Key1:
      status = HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin);
			break;
		case Key2:
			status = HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);
			break;
		case Key3:
			status = HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin);
			break;		
        case Key4:
			status = HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin);
			break;	
		default:
			break;
	}
	
	//硬件电平判断
	(status == 0) ? (status = KEY_ON) : (status = KEY_OFF);		
	
	return status;	
}
/*****************
按键执行回调函数--demo
******************/

void keyLP_key1()
{
	button = BUTTON1_LONG_PRESS;
}
void KeySP_key1()
{
	button = BUTTON1_SHORT_PRESS;
}
void keyLP_key2()
{
	button = BUTTON2_LONG_PRESS;
}
void KeySP_key2()
{
	button = BUTTON2_SHORT_PRESS;
}
void keyLP_key3()
{
	button = BUTTON2_LONG_PRESS;
}
void KeySP_key3()
{
	button = BUTTON3_SHORT_PRESS;
}

void KeySP_key4()
{
	button = BUTTON4_SHORT_PRESS;
}
void keyLP_key4()
{
	button = BUTTON4_LONG_PRESS;
}

 void KeySP_key5()
 {

 }
 void keyLP_key5()
 {

 }

void KeyGP_1()
{
	HAL_Delay(1);
}
void KeyGP_2()
{
	HAL_Delay(1);
}
/**************************************
			添加按键demo
****************************************/
void Add_Key_demo()
{
	unsigned char status = 0xFF;
	Key_info pp;
	
	pp.useFlg = KEY_USE;            
	pp.mode = KEY_CLOCK;
	pp.KeyLPRun = keyLP_key1;
	pp.KeySPRun = KeySP_key1;
	pp.KeyLOOPRun = NULL;
	status = Add_Change_Key(Key1,pp);
	
	if(status != ERROR_OK)
	{
		//OLED_ShowString(0,2,(uint8_t *)"ADD_KEY_ERROR",16);
	}	
	
	pp.useFlg = KEY_USE;            
	pp.mode = KEY_CLOCK;
	pp.KeyLPRun = keyLP_key2;
	pp.KeySPRun = KeySP_key2;
	pp.KeyLOOPRun = NULL;
	status = Add_Change_Key(Key2,pp);
	
	if(status != ERROR_OK)
	{
		//OLED_ShowString(0,2,(uint8_t *)"ADD_KEY_ERROR",16);
	}
	
//	pp.mode = KEY_CLOCK;
//	pp.KeyLPRun = keyLP_key3;
//	pp.KeySPRun = KeySP_key3;
//	pp.KeyLOOPRun = NULL;
//	status = Add_Change_Key(Key3,pp);
//	
//	if(status != ERROR_OK)
//	{
//		printf("添加按键失败3 Error代码  ---- 0x%2X\r\n",status);
//	}
    pp.useFlg = KEY_USE;            
	pp.mode = KEY_CLOCK;
	pp.KeyLPRun = keyLP_key3;
	pp.KeySPRun = KeySP_key3;
	pp.KeyLOOPRun = NULL;
	status = Add_Change_Key(Key3,pp);
	
	if(status != ERROR_OK)
	{
		//OLED_ShowString(0,2,(uint8_t *)"ADD_KEY_ERROR",16);
	}	
	
    	
	pp.useFlg = KEY_USE;
	pp.mode = KEY_CLOCK;
	pp.KeyLPRun = keyLP_key4;
	pp.KeySPRun = KeySP_key4;
	pp.KeyLOOPRun = NULL;
	status = Add_Change_Key(Key4, pp);

	if (status != ERROR_OK)
	{
		//OLED_ShowString(0, 2, (uint8_t*)"ADD_KEY_ERROR", 16);
	}
}

/**************************************
添加按键组合，必须先启动单键功能，不然会报错。
添加按键组合demo -- 组合模式下，取消单键的长按 ，短按操作
调用Del_KeyGP函数删除组合后，恢复各自单键的长按，短按操作
按键组合的按键可以复用，例如Key1和Key3组合后，Key1和Key2也能组合
****************************************/
void Add_KeyGP_demo()
{
	unsigned char status = 0xFF;
	KeyGP_info p;
	
	p.useFlg = KEYGP_USE;
	p.GPnum = 3;
	p.KeyNum[0] = Key1;
	p.KeyNum[1] = Key3;
	p.KeyNum[2] = Key2;
	p.KeyGPRun = KeyGP_1;
	p.GetKeyNumStrict = KEYSTRICT_ON;
	status = Add_Change_KeyGP(KeyGP1,p);
	if(status != ERROR_OK)
	{
		HAL_Delay(1);
	}
	
	p.KeyNum[1] = Key2;
	p.KeyGPRun = KeyGP_2;
	p.GetKeyNumStrict = KEYSTRICT_OFF;
	status = Add_Change_KeyGP(KeyGP2,p);
	if(status != ERROR_OK)
	{
		HAL_Delay(1);
	}	
		
	Del_KeyGP(KeyGP2);
	
}

void handleButtonState(void) 
{
	switch (button)
	{
	case BUTTON1_SHORT_PRESS:
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		// 处理按键1短按的操作
		button = WAITING;
		break;
	case BUTTON1_LONG_PRESS:
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		// 处理按键1长按的操作
		button = WAITING;
		break;
	case BUTTON2_SHORT_PRESS:
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		// 处理按键2短按的操作
		button = WAITING;
		break;
	case BUTTON2_LONG_PRESS:
		// 处理按键2长按的操作
		button = WAITING;
		break;
	case BUTTON3_SHORT_PRESS:
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		// 处理按键3短按的操作
		button = WAITING;
		break;
	case BUTTON3_LONG_PRESS:
		// 处理按键3长按的操作
		button = WAITING;
		break;
	case BUTTON4_SHORT_PRESS:
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		// 处理按键4短按的操作
		button = WAITING;
		break;
	case BUTTON4_LONG_PRESS:
		// 处理按键4长按的操作
		button = WAITING;
		break;
	case WAITING:
		// 处理等待状态的操作
		button = WAITING;
		break;
	default:
		button = WAITING;
		break;
	}
}

/****************用户修改区域 -- End***********************/

/*--------------------不需要修改	------------------------*/
void Key_Init()
{
	Key_BspInit();
	KeyInfo_Init();
}
/*--------------------------------------------------------*/

