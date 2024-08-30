#include "foc.h"
#include "pid.h"

#define MATH_PI 3.14159265358979323846f
#define MATH_2PI 6.28318530717958647693f
#define NUM_OF_POLE_PAIRS 7.0f
#define ANGLECOM 0x3FFF
#define ANGLECOM2 0x3FFE
//#define GI_D_KP (0.140358f)//1.0f Drag_IF_Mode
//#define GI_D_KI (706.3372224f)//24.0f  Drag_IF_Mode

//#define GI_Q_KP (0.1612076f)//1.0f Drag_IF_Mode
//#define GI_Q_KI (706.3372224f)//24.0f  Drag_IF_Mode
volatile float self_angle = 0,spi_pulse,mag_hudu, elec_hudu;
volatile float Id_ref = 0.0f, Iq_ref;
volatile float vel_cnt, vel_angle_0, vel_angle_1,vel,last_vel,low_alpha,vel_ref,vel_filt,last_vel_filt;
volatile float omega_vel, vel_delta_t = 0.0002f, xian_vel,xian_vel_filt,last_xian_vel,last_xian_vel_filt;
volatile float pos_cnt, pos_angle, last_pos_angle, pos_angle_360, pos_ref ,circle_count ,all_angle;

void FOC_Init(void)
{

}

//motor.mag_angle = (float)(AS5047_read(ANGLECOM) * MATH_2PI / 16384.0f);

//VFģʽ
void VF_RUN(void)
{
    //�Ƕ����
    self_angle += 0.5;
    if (self_angle > 360)
        self_angle = 0;

    theta_angle = self_angle;
    tran_angle(theta_angle);                //�Ƕ�ת����
    sin_cos_val();                          //���Ǳ任
    //����任
    Vq = 5.0f;
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
    //Vd = PID_realize_d(0.0f, Id);
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
//motor.mag_hudu = (float)((MT6816_Get_AngleData() + 192) * MATH_2PI / 16384.0f);
//motor.elec_hudu = Mag_To_Electrical(motor.mag_hudu, NUM_OF_POLE_PAIRS);


void Calibration_Zero(void)
{
    spi_pulse = AS5047_read(ANGLECOM2);
    //�Ƕ����
    self_angle += 1;
    if (self_angle > 360)
        self_angle = 0;

    theta_angle = 0;
    tran_angle(theta_angle);                //�Ƕ�ת����
    sin_cos_val();                          //���Ǳ任
    //����任
    Vq = 0.0f;
    Vd = 2.0f;
    anti_park_transf();                     //��תת��ֹ������
    svpwm_calc();                           //SVPWMת����

    TIM1->CCR1 = (uint16_t)(Tcmp1);
    TIM1->CCR2 = (uint16_t)(Tcmp2);
    TIM1->CCR3 = (uint16_t)(Tcmp3);
}
void Voltage_Open_Loop(void)
{
    //�Ƕ����
    spi_pulse = AS5047_read(ANGLECOM2);
    mag_hudu = (float)((spi_pulse + 138) * MATH_2PI / 16384.0f);
    elec_hudu = fmodf(mag_hudu * NUM_OF_POLE_PAIRS, MATH_2PI);
    //elec_hudu >= 0 ? elec_hudu : (elec_hudu + MATH_2PI);
    if(elec_hudu < 0)
        elec_hudu = elec_hudu + MATH_2PI;

    theta_hudu = elec_hudu;
    //tran_angle(theta_angle);                //�Ƕ�ת����
    sin_cos_val();                          //���Ǳ任
    //����任
    Vq = 3.0f;
    Vd = 0.0f;
    anti_park_transf();                     //��תת��ֹ������
    svpwm_calc();                           //SVPWMת����

    TIM1->CCR1 = (uint16_t)(Tcmp1);
    TIM1->CCR2 = (uint16_t)(Tcmp2);
    TIM1->CCR3 = (uint16_t)(Tcmp3);

    //��������
    clark_transf();
    park_transf();
}

void Current_Closed_Loop(void)
{
    //�Ƕ����
    spi_pulse = MT6816_Get_AngleData();
    mag_hudu = (float)((spi_pulse + 206) * MATH_2PI / 16384.0f);
    elec_hudu = fmodf(mag_hudu * NUM_OF_POLE_PAIRS, MATH_2PI);
    if (elec_hudu < 0)
        elec_hudu = elec_hudu + MATH_2PI;

    theta_hudu = elec_hudu;
    sin_cos_val();                          //���Ǳ任

    //��������
    clark_transf();
    park_transf();

    Iq_ref = -1.0f;
    Vd = -Pid_Cal(&GI_D, Id_ref, Id);
    Vq = Pid_Cal(&GI_Q, Iq_ref, Iq);
    //����任
    anti_park_transf();                     //��תת��ֹ������
    svpwm_calc();                           //SVPWMת����

    TIM1->CCR1 = (uint16_t)(Tcmp1);
    TIM1->CCR2 = (uint16_t)(Tcmp2);
    TIM1->CCR3 = (uint16_t)(Tcmp3);
}

void Velocity_Closed_Loop(void)
{
    //�Ƕ����
    spi_pulse = MT6816_Get_AngleData();
    mag_hudu = (float)((spi_pulse + 206) * MATH_2PI / 16384.0f);
    elec_hudu = fmodf(mag_hudu * NUM_OF_POLE_PAIRS, MATH_2PI);
    if (elec_hudu < 0)
        elec_hudu = elec_hudu + MATH_2PI;

    theta_hudu = elec_hudu;
    sin_cos_val();                          //���Ǳ任

    //��������
    clark_transf();
    park_transf();

    //�ٶȻ�ȡ
    if(vel_cnt == 0)
        vel_angle_0 = mag_hudu;
    else if(vel_cnt == 1)
        vel_angle_1 = mag_hudu;
    //�ٶȻ�ȡ���ٶȻ�
    if (vel_cnt == 1)
    {
        //spi_pulse��ÿ�����壬��Ӧ�Ļ���Ϊ0.00038330078125
        //������ȶ�Ӧ��rpm��18.310546875
        vel = (vel_angle_1 - vel_angle_0);
        //���ٶȼ���
        
        omega_vel = vel/vel_delta_t;
        //����㱣�� MATH_PI���ڿ��ܸ���
        if (vel > MATH_PI)
            omega_vel = (vel_angle_1 - vel_angle_0 - MATH_2PI) / vel_delta_t;
        else if (vel < -MATH_PI)
            omega_vel = (vel_angle_1 - vel_angle_0 + MATH_2PI) / vel_delta_t;
        //���ٶȼ���
        xian_vel = omega_vel * 60.0f / MATH_2PI;
        //���´������ϴ��ٶ�
      
                    //last_vel = vel;
                    //vel_cnt = 0;

                    //Iq_ref = Pid_Cal(&GVEL, vel_ref, vel);
                    //last_vel = vel;
        vel_ref = 160.0f;
        xian_vel_filt = LOW_PASS_FILTER(last_xian_vel_filt, xian_vel, 0.7);
        Iq_ref = Pid_Cal(&GVEL, vel_ref, xian_vel_filt);
        last_xian_vel_filt = xian_vel_filt;

        vel_cnt = 0;
    }
    else
    {
        vel_cnt = 1;
    }

    Vd = -Pid_Cal(&GI_D, Id_ref, Id);
    Vq = Pid_Cal(&GI_Q, Iq_ref, Iq);
    //���ʧ���ѹ
  //  if (SQUARE_ROOT_SUM(Vd, Vq) > 12.0f / SQRT3)
		//{
		//	while(1);
		//}
        //return;
    //����任
    anti_park_transf();                     //��תת��ֹ������
    svpwm_calc();                           //SVPWMת����

    TIM1->CCR1 = (uint16_t)(Tcmp1);
    TIM1->CCR2 = (uint16_t)(Tcmp2);
    TIM1->CCR3 = (uint16_t)(Tcmp3);
}

void Position_Closed_Loop(void)
{
    //�Ƕ����
    spi_pulse = MT6816_Get_AngleData();
    mag_hudu = (float)((spi_pulse + 206) * MATH_2PI / 16384.0f);
    elec_hudu = fmodf(mag_hudu * NUM_OF_POLE_PAIRS, MATH_2PI);
    if (elec_hudu < 0)
        elec_hudu = elec_hudu + MATH_2PI;

    theta_hudu = elec_hudu;
    sin_cos_val();                          //���Ǳ任

    //��������
    clark_transf();
    park_transf();

    //�ٶȻ�ȡ
    if (vel_cnt == 0)
        vel_angle_0 = mag_hudu;
    else if (vel_cnt == 1)
        vel_angle_1 = mag_hudu;
    //�ٶȻ�ȡ���ٶȻ�
    if (vel_cnt == 1)
    {
        //�ɴ�����ʮ�д��룬���ڱ�дλ�û�
        
        if (pos_cnt == 1)
        {
						HAL_GPIO_WritePin(CH2_GPIO_Port, CH2_Pin, GPIO_PIN_SET);
            //��ȡ�Ƕ�
            pos_angle = vel_angle_1;
            //����㱣��     //��ʱ�����ϵ���ǹ��������
                //����ϵ����0��
                //����ϵ绡�ȳ���PI����ֱ�ӽ���Ȧ��+1����������Ҳ�ñ���
            if (pos_angle - last_pos_angle > MATH_PI)
            {
                circle_count--;
                //pos_angle = 6
                 //last_pos_angle = 1
                 //6 - 1 > 3.141592653589793
                 //6.28*circle(-1) + pos_angle
            }
            else if (pos_angle - last_pos_angle < -MATH_PI)
            {
                circle_count++;
                //pos_angle = 1
                //last_pos_angle = 5
                //1 - 5 < -3.141592653589793
                //6.28*circle(+1) + pos_angle
            }
            last_pos_angle = pos_angle;
            //�Ƕ�ת����
            pos_angle_360 = (circle_count * MATH_2PI + pos_angle) * 360.0f / MATH_2PI;
            //if(vel_angle_1 - vel_angle_0 > MATH_PI)
            //λ�û�
            //all_angle = (circle_count * MATH_2PI + pos_angle);
            pos_ref = -90.0f;
            float pid_output = Pid_Cal(&GPOS, pos_ref, pos_angle_360);
            if (pid_output > 0)
                vel_ref = pid_output;
            else
                vel_ref = pid_output;
				
            pos_cnt = 0;
						HAL_GPIO_WritePin(CH2_GPIO_Port, CH2_Pin, GPIO_PIN_RESET);
        }
        else if (pos_cnt == 0)
        {
            pos_cnt = 1;
        }










        //spi_pulse��ÿ�����壬��Ӧ�Ļ���Ϊ0.00038330078125
        //������ȶ�Ӧ��rpm��18.310546875
        vel = (vel_angle_1 - vel_angle_0);
        //���ٶȼ���

        omega_vel = vel / vel_delta_t;
        //����㱣�� MATH_PI���ڿ��ܸ���
        if (vel > MATH_PI)
            omega_vel = (vel_angle_1 - vel_angle_0 - MATH_2PI) / vel_delta_t;
        else if (vel < -MATH_PI)
            omega_vel = (vel_angle_1 - vel_angle_0 + MATH_2PI) / vel_delta_t;
        //���ٶȼ���
        xian_vel = omega_vel * 60.0f / MATH_2PI;
        //���´������ϴ��ٶ�

                    //last_vel = vel;
                    //vel_cnt = 0;

                    //Iq_ref = Pid_Cal(&GVEL, vel_ref, vel);
                    //last_vel = vel;
        
        xian_vel_filt = LOW_PASS_FILTER(last_xian_vel_filt, xian_vel, 0.7);
        Iq_ref = Pid_Cal(&GVEL, vel_ref, xian_vel_filt);
        last_xian_vel_filt = xian_vel_filt;

        vel_cnt = 0;
    }
    else
    {
        vel_cnt = 1;
    }

    Vd = -Pid_Cal(&GI_D, Id_ref, Id);
    Vq = Pid_Cal(&GI_Q, Iq_ref, Iq);
    //���ʧ���ѹ
  //  if (SQUARE_ROOT_SUM(Vd, Vq) > 12.0f / SQRT3)
        //{
        //	while(1);
        //}
        //return;
    //����任
    anti_park_transf();                     //��תת��ֹ������
    svpwm_calc();                           //SVPWMת����

    TIM1->CCR1 = (uint16_t)(Tcmp1);
    TIM1->CCR2 = (uint16_t)(Tcmp2);
    TIM1->CCR3 = (uint16_t)(Tcmp3);
}