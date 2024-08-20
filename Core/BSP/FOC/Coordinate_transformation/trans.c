#include "trans.h"
volatile float theta_angle;
volatile float theta_hudu;
volatile float sin_val;
volatile float cos_val;

volatile float Iq, Id, Ialpha, Ibeta;
volatile float Vd, Vq, Valpha, Vbeta;
volatile float Ia, Ib, Ic;
volatile float Tpwm_temp, Udc_temp;
volatile float Tcmp1, Tcmp2, Tcmp3;

//�Ƕ����������󴫸�tran_angle()���ٵ���sin_cos_val()���������÷�PARK�任��������SVPWM���㣬Vq��VdҲ��Ҫ��ǰָ����

void tran_angle(float angle)
{
    theta_angle = angle;
	theta_hudu = theta_angle * PI / 180.0F;
}

//sinf(x),����xӦΪ����ֵ���������xΪ�Ƕ�ֵ��Ӧ����(��/180)ת���ɶ�Ӧ�Ļ���ֵ��
void sin_cos_val()
{
	sin_val = sinf(theta_hudu);
	cos_val = cosf(theta_hudu);
}

/***************************************
���ܣ�PARK�任
�βΣ�alpha_beta������COS_SINֵ��DQ�����
˵����������ֱ��
***************************************/
//Id = Ialpha*cos(theta) + Ibeta*sin(theta)
//Iq = -Ialpha*sin(theta) + Ibeta*cos(theta)

void park_transf()
{
	Iq = Ialpha * cos_val + Ibeta * sin_val;
	Id = -Ialpha * sin_val + Ibeta * cos_val;
    //Iq =-Iq; //������ֱ��
    //Id = -Id;
}
/***************************************
���ܣ���PARK�任
�βΣ�DQ���ѹ��COS_SINֵ��alpha_beta��ѹ
˵����ֱ���佻��
***************************************/
//Valpha = cos(theta)*Vd - sin(theta)*Vq
//Vbeta = sin(theta)*Vd + cos(theta)*Vq
void anti_park_transf()
{
	Valpha = cos_val * Vd - sin_val * Vq;
	Vbeta = sin_val * Vd + cos_val * Vq;
}

/***************************************
���ܣ�Clark�任
�βΣ���������Լ�alpha_beta����
˵���������໥��120�ȱ任�����໥��90��
***************************************/
//Ialpha = (Ia - (Ib + Ic)/2)*2/3
//Ibeta = (Ib - Ic)*sqrt(3)/2*2/3
void clark_transf()
{
	Ialpha = (Ia - (Ib + Ic) * 0.5F) * 2.0F / 3.0F;
	Ibeta = (Ib - Ic) * SQRT3 / 2 * 2.0F / 3.0F;
}

/***************************************
���ܣ�SVPWM����
�βΣ�alpha_beta��ѹ�Լ�ĸ�ߵ�ѹ����ʱ������
˵��������alpha_beta��ѹ��������ռ�ձ�
***************************************/

void svpwm_calc()
{
    Udc_temp = 12.0F; //ĸ�ߵ�ѹ
    Tpwm_temp = __HAL_TIM_GET_AUTORELOAD(&htim1); //���㶨ʱ������
    int sector;
    float Tx, Ty, f_temp, Ta, Tb, Tc;     //����Tcmp�ǱȽ�ֵ��sector������Nֵ
    sector = 0;
    Tcmp1 = 0.0F;
    Tcmp2 = 0.0F;
    Tcmp3 = 0.0F;
    /*=== �����ж� ===*/
    if (Vbeta > 0.0F) 
    {
        sector = 1;
    }

    if ((SQRT3 * Valpha - Vbeta) / 2.0F > 0.0F) 
    {
        sector += 2;
    }

    if ((-SQRT3 * Valpha - Vbeta) / 2.0F > 0.0F) 
    {
        sector += 4;
    }

    /*=== ʸ������ʱ����� ===*/
    switch (sector) //�����Tx��Ty
    {
        case 1:
            Tx = (-1.5F * Valpha + SQRT3 / 2 * Vbeta) * (Tpwm_temp / Udc_temp);
            Ty = (1.5F * Valpha + SQRT3 / 2 * Vbeta) * (Tpwm_temp / Udc_temp);
            break;

        case 2:
            Tx = (1.5F * Valpha + SQRT3 / 2 * Vbeta) * (Tpwm_temp / Udc_temp);
            Ty = -(SQRT3 * Vbeta * Tpwm_temp / Udc_temp);
            break;

        case 3:
            Tx = -((-1.5F * Valpha + SQRT3 / 2 * Vbeta) * (Tpwm_temp / Udc_temp));
            Ty = SQRT3 * Vbeta * Tpwm_temp / Udc_temp;
            break;

        case 4:
            Tx = -(SQRT3 * Vbeta * Tpwm_temp / Udc_temp);
            Ty = (-1.5F * Valpha + SQRT3 / 2 * Vbeta) * (Tpwm_temp / Udc_temp);
            break;

        case 5:
            Tx = SQRT3 * Vbeta * Tpwm_temp / Udc_temp;
            Ty = -((1.5F * Valpha + SQRT3 / 2 * Vbeta) * (Tpwm_temp / Udc_temp));
            break;

        default:
            Tx = -((1.5F * Valpha + SQRT3 / 2 * Vbeta) * (Tpwm_temp / Udc_temp));
            Ty = -((-1.5F * Valpha + SQRT3 / 2 * Vbeta) * (Tpwm_temp / Udc_temp));
            break;
    }
    /*=== ��һ�������е�󣬸ĳ�ժҪ�е� ===*/
    /*
    f_temp = Tx + Ty;
    if (f_temp > Tpwm_temp)
    {
      Tx /= f_temp;
      Ty /= (Tx + Ty);
    }
    */
    //�޷�
    f_temp = Tx + Ty;
    if (f_temp > Tpwm_temp)
    {
        Tx = Tx / f_temp * Tpwm_temp;
        Ty = Ty / f_temp * Tpwm_temp;
    }

    //����Ta��Tb��Tc
    Ta = (Tpwm_temp - (Tx + Ty)) / 4.0F;
    Tb = Tx / 2.0F + Ta;
    Tc = Ty / 2.0F + Tb;    //�˴����Կ�������Ϊ�ߵ�ƽ����ʱ��
    
    switch (sector) //Ta��Tb��Tc���������ͨ��
    {
        case 1:
            Tcmp1 = Tb;
            Tcmp2 = Ta;
            Tcmp3 = Tc;
            break;

        case 2:
            Tcmp1 = Ta;
            Tcmp2 = Tc;
            Tcmp3 = Tb;
            break;

        case 3:
            Tcmp1 = Ta;
            Tcmp2 = Tb;
            Tcmp3 = Tc;
            break;

        case 4:
            Tcmp1 = Tc;
            Tcmp2 = Tb;
            Tcmp3 = Ta;
            break;

        case 5:
            Tcmp1 = Tc;
            Tcmp2 = Ta;
            Tcmp3 = Tb;
            break;

        case 6:
            Tcmp1 = Tb;
            Tcmp2 = Tc;
            Tcmp3 = Ta;
            break;
    }
    /*=== ʩ�ӱȽ�ֵ����ʱ�� ===*/
    //FOC_Output.Tcmp1 = Tcmp1;
    //FOC_Output.Tcmp2 = Tcmp2;
    //FOC_Output.Tcmp3 = Tcmp3;
}