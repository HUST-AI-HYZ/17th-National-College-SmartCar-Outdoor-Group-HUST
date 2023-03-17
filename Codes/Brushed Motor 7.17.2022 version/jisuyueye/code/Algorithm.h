#ifndef __ALGORITHNM_H
#define __ALGORITHNM_H

#include "zf_common_headfile.h"

extern float angle, angle_dot; 	
void Kalman_Filter(float Accel,float Gyro);		
float Yijielvbo(float angle_m, float gyro_m);
#endif
