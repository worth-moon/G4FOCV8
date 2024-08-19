#include "zengliang_pid.h"

__pid pid4;
void PID_param_init_zls() 
{
    // ��ʼ������ //
    //printf("PID-init begin \n");
    pid4.target_val = 0.0;
    pid4.actual_val = 0.0;
    pid4.err = 0.0;
    pid4.err_last = 0.0;
    pid4.err_next = 0.0;
    pid4.Kp = 0.21;
    pid4.Ki = 0.80;
    pid4.Kd = 0.01;
    //printf("PID_init end \n");
}

float PID_realize_zls(float temp_val) 
{
    // Ŀ��ֵֻ�����������㣬����Ŀ��ֵ��ʵ��ֵ����� *
    pid4.err = pid4.target_val - temp_val;
    /*PID �㷨ʵ�� */
    float increment_val = pid4.Kp * (pid4.err - pid4.err_next) + pid4.Ki * pid4.err +
        pid4.Kd * (pid4.err - 2 * pid4.err_next + pid4.err_last);

    // �ۼ� *
    pid4.actual_val += increment_val;

    // ������� *
    pid4.err_last = pid4.err_next;
    pid4.err_next = pid4.err;

    // ���ص��Ǿ��� pid4 �����Ժ��ֵ *
    return pid4.actual_val;
}