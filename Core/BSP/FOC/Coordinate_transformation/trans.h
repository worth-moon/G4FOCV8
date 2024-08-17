#ifndef __TRANS_H
#define __TRANS_H
#include "main.h"
#include "math.h"

#define SQRT3 1.7320508075688772935274463415059F
#define PI 3.1415926535897932384626433832795F

extern float theta_angle;

void tran_angle(float angle);
void sin_cos_val();
void park_transf();
void anti_park_transf();
void svpwm_calc();
#endif
