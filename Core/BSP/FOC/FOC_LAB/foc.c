#include "foc.h"

#define MATH_PI 3.14159265358979323846f
#define MATH_2PI 6.28318530717958647693f
#define NUM_OF_POLE_PAIRS 7.0f

float self_angle = 0,spi_pulse,mag_hudu, elec_hudu;

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

    theta_angle = self_angle;
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

void IF_RUN(void)
{
    //角度相关
    self_angle += 0.5;
    if (self_angle > 360)
        self_angle = 0;

    theta_angle = self_angle;
    tran_angle(theta_angle);                //角度转弧度
    sin_cos_val();                          //三角变换

    //电流采样
    clark_transf();
    park_transf();
    //坐标变换
    Vq = PID_realize_q(1.0f,Iq);  //Vq为0.5的时候，Iq大概在1A左右
    //Vq = 0.5f;
    //Vd = PID_realize_q(0.0f, Id);
    Vd = 0.0f;
    //pid限幅
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
    //坐标变换
    anti_park_transf();                     //旋转转静止坐标轴
    svpwm_calc();                           //SVPWM转三相
    anti_park_transf();                     //旋转转静止坐标轴
    svpwm_calc();                           //SVPWM转三相

    TIM1->CCR1 = (uint16_t)(Tcmp1);
    TIM1->CCR2 = (uint16_t)(Tcmp2);
    TIM1->CCR3 = (uint16_t)(Tcmp3);
}
//motor.mag_hudu = (float)((MT6816_Get_AngleData() + 192) * MATH_2PI / 16384.0f);
//motor.elec_hudu = Mag_To_Electrical(motor.mag_hudu, NUM_OF_POLE_PAIRS);


void Calibration_Zero(void)
{
    spi_pulse = MT6816_Get_AngleData();
    //角度相关
    self_angle += 1;
    if (self_angle > 360)
        self_angle = 0;

    theta_angle = 0;
    tran_angle(theta_angle);                //角度转弧度
    sin_cos_val();                          //三角变换
    //坐标变换
    Vq = 0.0f;
    Vd = 0.5f;
    anti_park_transf();                     //旋转转静止坐标轴
    svpwm_calc();                           //SVPWM转三相

    TIM1->CCR1 = (uint16_t)(Tcmp1);
    TIM1->CCR2 = (uint16_t)(Tcmp2);
    TIM1->CCR3 = (uint16_t)(Tcmp3);
}
void Voltage_Open_Loop(void)
{

    //角度相关
    spi_pulse = MT6816_Get_AngleData();
    mag_hudu = (float)((spi_pulse + 206) * MATH_2PI / 16384.0f);
    elec_hudu = fmodf(mag_hudu * NUM_OF_POLE_PAIRS, MATH_2PI);
    //elec_hudu >= 0 ? elec_hudu : (elec_hudu + MATH_2PI);
    if(elec_hudu < 0)
        elec_hudu = elec_hudu + MATH_2PI;

    theta_hudu = elec_hudu;
    //tran_angle(theta_angle);                //角度转弧度
    sin_cos_val();                          //三角变换
    //坐标变换
    Vq = 0.5f;
    Vd = 0.0f;
    anti_park_transf();                     //旋转转静止坐标轴
    svpwm_calc();                           //SVPWM转三相

    TIM1->CCR1 = (uint16_t)(Tcmp1);
    TIM1->CCR2 = (uint16_t)(Tcmp2);
    TIM1->CCR3 = (uint16_t)(Tcmp3);

    //电流采样
    clark_transf();
    park_transf();
}

