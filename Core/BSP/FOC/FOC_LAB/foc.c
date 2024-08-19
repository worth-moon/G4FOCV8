#include "foc.h"

float self_angle = 0;

void FOC_Init(void)
{

}


//VFģʽ
void VF_RUN(void)
{
    //�Ƕ����
    self_angle += 1;
    if (self_angle > 360)
        self_angle = 0;

    theta_angle = self_angle;
    tran_angle(theta_angle);                //�Ƕ�ת����
    sin_cos_val();                          //���Ǳ任
    //����任
    Vq = 0.5f;
    Vd = 0.0f;
    anti_park_transf();                     //��תת��ֹ������
    svpwm_calc();                           //SVPWMת����

    TIM1->CCR1 = (uint16_t)(Tcmp1);
    TIM1->CCR2 = (uint16_t)(Tcmp2);
    TIM1->CCR3 = (uint16_t)(Tcmp3);
}

void IF_RUN(void)
{
    //�Ƕ����
    self_angle += 0.5;
    if (self_angle > 360)
        self_angle = 0;

    theta_angle = self_angle;
    tran_angle(theta_angle);                //�Ƕ�ת����
    sin_cos_val();                          //���Ǳ任

    //��������
    clark_transf();
    park_transf();
    //����任
    Vq = PID_realize_q(1.0f,Iq);  //VqΪ0.5��ʱ��Iq�����1A����
    //Vq = 0.5f;
    //Vd = PID_realize_q(0.0f, Id);
    Vd = 0.0f;
    //pid�޷�
    float max_Vq = 0.5f;
    float max_Vd = 0.5f;
    if(Vq > max_Vq)
        Vq = max_Vq;
    if(Vq < -max_Vq)
        Vq = -max_Vq;
    if(Vd > max_Vd)
        Vd = max_Vd;
    if(Vd < -max_Vd)
        Vd = -max_Vd;
    //����任
    anti_park_transf();                     //��תת��ֹ������
    svpwm_calc();                           //SVPWMת����
    anti_park_transf();                     //��תת��ֹ������
    svpwm_calc();                           //SVPWMת����

    TIM1->CCR1 = (uint16_t)(Tcmp1);
    TIM1->CCR2 = (uint16_t)(Tcmp2);
    TIM1->CCR3 = (uint16_t)(Tcmp3);
}
