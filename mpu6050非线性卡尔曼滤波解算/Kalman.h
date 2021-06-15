#ifndef _Kalman_H_
#define	_Kalman_H_

#include "stm32f4xx_hal.h"
#include "math.h"

#define GYRO_TO_RAD(x) ((float)x*0.0001331580545f) //gyro*250/2^15/57.3
#define GYRO_TO_DEG(x) ((float)x*0.0076293945f) //gyro*250/2^15

void IMUupdate(short *gx,short *gy,short *gz,short ax,short ay,short az,float *roll,float *pitch,float *yaw);

#endif
