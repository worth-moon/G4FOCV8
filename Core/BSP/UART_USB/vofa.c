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
	ch1 = (float)TIM1->CCR1;
	ch2 = (float)TIM1->CCR2;
	ch3 = (float)TIM1->CCR3;
// 	vofa_send_data(0, spi_pulse);
//	vofa_send_data(1, pos_angle_360);
//	vofa_send_data(2, pos_ref);
	vofa_send_data(0, self_angle);
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












