#ifndef __POSITION_PID_H__
#define __POSITION_PID_H__

#include "main.h"

typedef struct {
    float target_val;        //Ŀ��ֵ
    float actual_val;        //ʵ��ֵ
    float err;               //ƫ��ֵ
    float err_last;          //��һ��ƫ��ֵ
    float Kp, Ki, Kd;        //���������֡�΢��ϵ��
    float integral;          //����ֵ
} _pid;


void PID_param_init();
float PID_realize_q(float target,float temp_val);
float PID_realize_d(float target,float temp_val);

#endif 
