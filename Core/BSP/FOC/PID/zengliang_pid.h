#ifndef __ZENGLIANG_PID_H__
#define __ZENGLIANG_PID_H__

#include "main.h"
#include "position_pid.h"
typedef struct {
    float target_val;        //Ŀ��ֵ
    float actual_val;        //ʵ��ֵ
    float err;               //��ǰƫ��ֵ
    float err_next;          //��һ��ƫ��ֵ
    float err_last;          //���һ��ƫ��ֵ
    float Kp, Ki, Kd;        //���������֡�΢��ϵ��
} __pid;

void PID_param_init_zls();
float PID_realize_zls(float temp_val);

#endif 


