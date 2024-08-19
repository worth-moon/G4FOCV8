#include "foc.h"

float self_angle = 0;

void FOC_Init(void)
{

}


//VF模式
void VF_RUN(void)
{
    //角度相关
    self_angle += 1;
    if (self_angle > 360)
        self_angle = 0;

    theta_angle = -self_angle;
    tran_angle(theta_angle);                //角度转弧度
    sin_cos_val();                          //三角变换
    //坐标变换
    Vq = 0.5f;
    Vd = 0.0f;
    anti_park_transf();                     //旋转转静止坐标轴
    svpwm_calc();                           //SVPWM转三相

    TIM1->CCR1 = (uint16_t)(Tcmp1);
    TIM1->CCR2 = (uint16_t)(Tcmp2);
    TIM1->CCR3 = (uint16_t)(Tcmp3);
}