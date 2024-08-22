#include "vofa.h"
#include "usbd_cdc_if.h"
#include "math.h"
//#include "usart.h"
#include "trans.h"
uint8_t send_buf[MAX_BUFFER_SIZE];
uint16_t cnt = 0;



/*================================= Justfloat =====================================*/
/*
1. vofa_demo()					循环调用
2. vofa_transmit				通信接口
3. vofa_send_data				发送接口
4. vofa_sendframetail			发送帧尾
*/
/**
***********************************************************************
* @brief:      vofa_transmit(uint8_t* buf, uint16_t len)
* @param:		   void
* @retval:     void
* @details:    修改通信工具，USART或者USB
***********************************************************************
**/
void vofa_transmit(uint8_t* buf, uint16_t len)
{
	//HAL_UART_Transmit(&VOFA_UART, (uint8_t *)buf, len, 0xFFFF);
	CDC_Transmit_FS((uint8_t *)buf, len);
}
/**
***********************************************************************
* @brief:      vofa_send_data(float data)
* @param[in]:  num: 数据编号 data: 数据 
* @retval:     void
* @details:    将浮点数据拆分成单字节
***********************************************************************
**/
void vofa_send_data(uint8_t num, float data) 
{
	send_buf[cnt++] = byte0(data);
	send_buf[cnt++] = byte1(data);
	send_buf[cnt++] = byte2(data);
	send_buf[cnt++] = byte3(data);
}
/**
***********************************************************************
* @brief      vofa_sendframetail(void)
* @param      NULL 
* @retval     void
* @details:   给数据包发送帧尾
***********************************************************************
**/
void vofa_sendframetail(void) 
{
	send_buf[cnt++] = 0x00;
	send_buf[cnt++] = 0x00;
	send_buf[cnt++] = 0x80;
	send_buf[cnt++] = 0x7f;
	
	/* 将数据和帧尾打包发送 */
	vofa_transmit((uint8_t *)send_buf, cnt);
	cnt = 0;// 每次发送完帧尾都需要清零
}
/**
***********************************************************************
* @brief      vofa_demo(void)
* @param      NULL 
* @retval     void
* @details:   demo示例
***********************************************************************
**/
void vofa_demo(void) 
{
//	static float scnt = 0.0f;

//	scnt += 1.0f;

//	if(scnt >= 360.0f)
//		scnt = 0.0f;

//	//float v1 = scnt;
//	//float v2 = sin((double)scnt / 180 * 3.14159) * 180 + 180;
//	//float v3 = sin((double)(scnt + 120) / 180 * 3.14159) * 180 + 180;
//	//float v4 = sin((double)(scnt + 240) / 180 * 3.14159) * 180 + 180;

//    uint16_t t4, t6, t0;
//    uint8_t section;							//扇区
//    uint16_t Ts = __HAL_TIM_GET_AUTORELOAD(&htim1)/10;	//计时器周期
//    
//    int angle = (int)scnt;
//		float m = 0.5f;

//    section = angle / 60 + 1;					//得到角度对应的扇区
//    angle %= 60;								//因为前面的算法只计算了0到60度

//    /*得到矢量的作用时间，除以57.2958是把角度换算成弧度*/
//    t4 = sinf((60 - angle) / 57.2958f) * Ts * m;
//    t6 = sinf(angle / 57.2958f) * Ts * m;
//    t0 = (Ts - t4 - t6) / 2;

//    /*判断扇区，用7段式svpwm调制，得到三个通道的装载值*/
//    switch (section)
//    {
//    case 1:
//    {
//        ch1 = t4 + t6 + t0;
//        ch2 = t6 + t0;
//        ch3 = t0;
//    }break;

//    case 2:
//    {
//        ch1 = t4 + t0;
//        ch2 = t4 + t6 + t0;
//        ch3 = t0;
//    }break;

//    case 3:
//    {
//        ch1 = t0;
//        ch2 = t4 + t6 + t0;
//        ch3 = t6 + t0;
//    }break;

//    case 4:
//    {
//        ch1 = t0;
//        ch2 = t4 + t0;
//        ch3 = t4 + t6 + t0;
//    }break;

//    case 5:
//    {
//        ch1 = t6 + t0;
//        ch2 = t0;
//        ch3 = t4 + t6 + t0;
//    }break;

//    case 6:
//    {
//        ch1 = t4 + t6 + t0;
//        ch2 = t0;
//        ch3 = t4 + t0;
//    }break;

//    default:
//        break;
//    }

//    TIM1->CCR1 = (uint16_t)ch1;
//    TIM1->CCR2 = (uint16_t)ch2;
//    TIM1->CCR3 = (uint16_t)ch3;
// 
	ch1 = (float)TIM1->CCR1;
	ch2 = (float)TIM1->CCR2;
	ch3 = (float)TIM1->CCR3;
 	vofa_send_data(0, GVEL.i);
	vofa_send_data(1, pos_angle_360);
	vofa_send_data(2, pos_ref);
	// Call the function to store the data in the buffer
	vofa_send_data(3, Ia);
	vofa_send_data(4, Ib);
	vofa_send_data(5, Ic);
	vofa_send_data(6, Ialpha);
	vofa_send_data(7, Ibeta);
	vofa_send_data(8, Id);
	vofa_send_data(9, Iq);

	vofa_send_data(10, Vd);
	vofa_send_data(11, Vq);
	vofa_send_data(12, xian_vel);
	vofa_send_data(13, xian_vel_filt);
	vofa_send_data(14, Iq_ref);
	vofa_send_data(15, vel_ref);
	vofa_sendframetail();
}



//float vofa_cmd_parse(uint8_t *cmdBuf, char *arg)
//{
//    return atof(cmdBuf + strlen(arg));
//}

//uint8_t receive_buf[100];
//void vofa_Receive(uint8_t* Buf,uint16_t *Len)
//{
//    memcpy(receive_buf,Buf,*Len);
//    
//    if(strstr(receive_buf,"set_cq_p = "))
//    {
//        //GI_Q.kp = vofa_cmd_parse(receive_buf, "set_cq_p = " );
//    }
//		
//	if(strstr(receive_buf,"set_cq_i = "))
//    {
//        //GI_Q.ki = vofa_cmd_parse(receive_buf, "set_cq_i = " );
//    }
//}












